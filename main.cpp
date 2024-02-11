#define _CRT_SECURE_NO_WARNINGS

#include <filesystem>
#include <fstream>
#include <iostream>

#include <ulib/process.h>
#include <ulib/strutility.h>

#include <futile/futile.h>

#include <nlohmann/json.hpp>

#include <Windows.h>

#include <3rdparty/WinReg.hpp>

int main(int argc, char **argv)
{
    auto fail_with_error = [](const auto &error, const auto &code = "unknown_error", int exit_code = -1) {
        std::cerr << nlohmann::json::object({{"error_code", code}, {"error_message", error}}).dump() << std::endl;
        return exit_code;
    };

    try
    {
        std::filesystem::path path_to_this = argv[0];
        path_to_this = path_to_this.parent_path();

        auto vswhere_exe = path_to_this / "vswhere.exe";

        if (!std::filesystem::exists(vswhere_exe))
        {
            // Find the globally available vswhere.exe.
            std::filesystem::path program_files_x86 = std::getenv("ProgramFiles(x86)");
            vswhere_exe = program_files_x86 / "Microsoft Visual Studio/Installer/vswhere.exe";
        }

        if (!std::filesystem::exists(vswhere_exe))
        {
            // If it STILL hasn't been found, exit with an error
            return fail_with_error("Failed to find 'vswhere.exe'! You probably don't have Visual Studio.",
                                   "vswhere_not_found");
        }

        ulib::process vswhere{vswhere_exe,
                              {"-latest", "-nocolor", "-utf8", "-format", "json"},
                              ulib::process::die_with_parent | ulib::process::pipe_stdout | ulib::process::pipe_stderr};

        if (vswhere.wait() != 0)
        {
            auto vswhere_error_output = ulib::sstr(vswhere.err().read_all());
            return fail_with_error("vswhere failed: " + vswhere_error_output, "vswhere_failed");
        }

        auto vswhere_stuff_u = vswhere.out().read_all();
        vswhere_stuff_u.MarkZeroEnd();

        auto vswhere_stuff_text = ulib::u8(vswhere_stuff_u);

        auto vswhere_stuff = nlohmann::json::parse(vswhere_stuff_text);

        if (vswhere_stuff.empty())
        {
            return fail_with_error("No Visual Studio installations were found on this computer", "vs_not_found");
        }

        const auto &vs_info = vswhere_stuff[0];
        std::filesystem::path vs_path = vs_info["installationPath"].get<std::string>();

        constexpr auto vctoolsversion_path = "VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt";
        auto vc_tools_version = futile::open(vs_path / vctoolsversion_path).lines<std::string>().front();

        auto vc_tools_dir = vs_path / "VC/Tools/MSVC/" / vc_tools_version;

        winreg::RegKey winsdk_node;

        winsdk_node.Create(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0",
                           KEY_READ | KEY_WOW64_32KEY);

        std::filesystem::path winsdk_directory =
            ulib::sstr(ulib::u8(winsdk_node.GetStringValue(L"InstallationFolder")));
        auto winsdk_version = ulib::sstr(ulib::u8(winsdk_node.GetStringValue(L"ProductVersion")));

        winsdk_node.Close();

        winsdk_version.append(".0"); // TODO: figure out what it means!

        ulib::list<std::filesystem::path> default_include_dirs;

        default_include_dirs.push_back(vc_tools_dir / "include");
        default_include_dirs.push_back(vc_tools_dir / "ATLMFC" / "include");
        default_include_dirs.push_back(vs_path / "VC/Auxiliary/VS/include");

        auto winsdk_include_root = winsdk_directory / "include" / winsdk_version;

        for (auto &entry : std::filesystem::directory_iterator{winsdk_include_root})
        {
            if (entry.is_directory())
                default_include_dirs.push_back(entry.path());
        }

        auto winsdk_bin_root = winsdk_directory / "bin" / winsdk_version;

        auto result = nlohmann::json::object();

        result["vs_install_dir"] = vs_path.generic_u8string();

        result["vc_tools_version"] = vc_tools_version;
        result["vc_tools_dir"] = vc_tools_dir.generic_u8string();

        result["winsdk_directory"] = winsdk_directory.generic_u8string();
        result["winsdk_version"] = winsdk_version;
        result["winsdk_bin"] = winsdk_bin_root.generic_u8string();

        ulib::list<ulib::u8string> include_dirs;

        for (auto &path : default_include_dirs)
        {
            if (std::filesystem::exists(path))
            {
                include_dirs.push_back(path.generic_u8string());
            }
        }

        result["vc_include_dirs"] = include_dirs;

        // Generate the necessary environment variables
        auto &env = result["environment"];

        env["VCToolsInstallDir"] = result["vc_tools_dir"];
        env["VCToolsVersion"] = result["vc_tools_version"];
        env["WindowsSdkDir"] = result["winsdk_directory"];
        env["WindowsSDKLibVersion"] = result["winsdk_version"];
        env["WindowsSDKVersion"] = result["winsdk_version"];
        env["WindowsSdkBinPath"] = (winsdk_directory / "bin").generic_u8string();
        env["WindowsSdkVerBinPath"] = result["winsdk_bin"];
        env["INCLUDE"] = ulib::join(include_dirs, ";");

        std::cout << result.dump(4) << std::endl;

        return 0;
    }
    catch (const std::exception &ex)
    {
        return fail_with_error(ex.what(), typeid(ex).name(), -127);
    }
    catch (...)
    {
        return fail_with_error("error", "unknown", -127);
    }
}
