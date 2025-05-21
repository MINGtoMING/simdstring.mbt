import platform
import subprocess
import json
import os


def get_intruction_list():
    os_type = platform.system().lower()
    arch = platform.machine().lower()
    flags = []

    try:
        if os_type == "linux":
            if "x86" in arch:
                with open("/proc/cpuinfo", "r") as f:
                    for line in f:
                        if line.startswith("flags"):
                            parts = line.split(":", 1)
                            flags_str = parts[1].strip().lower().replace(".", "_")
                            flags = flags_str.split()
                            break
        elif os_type == "windows":
            output = subprocess.check_output(
                ["wmic", "cpu", "get", "flags", "/format:list"],
                stderr=subprocess.STDOUT,
                text=True,
                creationflags=subprocess.CREATE_NO_WINDOW,
            )
            for line in output.splitlines():
                if line.startswith("Flags="):
                    flags_str = line.split("=", 1)[1].strip().lower().replace(".", "_")
                    flags = flags_str.split()
                    break
        elif os_type == "darwin":
            features_output = subprocess.check_output(
                ["sysctl", "machdep.cpu.features"], text=True
            )
            leaf7_output = subprocess.check_output(
                ["sysctl", "machdep.cpu.leaf7_features"], text=True
            )
            for line in features_output.splitlines():
                if line.startswith("machdep.cpu.features:"):
                    parts = line.split(":", 1)
                    if len(parts) >= 2:
                        features_str = parts[1].strip().lower().replace(".", "_")
                        flags.extend(features_str.split())
            for line in leaf7_output.splitlines():
                if line.startswith("machdep.cpu.leaf7_features:"):
                    parts = line.split(":", 1)
                    if len(parts) >= 2:
                        leaf7_str = parts[1].strip().lower().replace(".", "_")
                        flags.extend(leaf7_str.split())
    except Exception:
        return []

    return list(set(flags))


def update_config(config_path: str, instruction: str):
    with open(config_path, "r") as f:
        config = json.load(f)
    with open(config_path, "w") as f:
        config["import"] = [
            {
                "path": f"MINGtoMING/simdstring/{instruction}",
                "alias": "internel",
            }
        ]
        json.dump(config, f, indent=4)


def main():
    instruction_list = get_intruction_list()
    if "avx2" in instruction_list:
        update_config("moon.pkg.json", "avx2")
    elif "sse4_2" in instruction_list:
        update_config("moon.pkg.json", "sse4_2")
    else:
        update_config("moon.pkg.json", "scalar")

if __name__ == "__main__":
    main()
