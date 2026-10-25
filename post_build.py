Import("env")
import os
import shutil

def after_build(source, target, env):
    # Pobieramy ścieżkę do świeżo skompilowanego pliku .bin
    target_bin = str(target[0])
    project_dir = env.get("PROJECT_DIR")
    
    app_version = "0.0.0"
    device_type = "UNKNOWN_DEVICE"

    # Przeszukujemy flagi kompilatora w poszukiwaniu nazwy i wersji
    for define in env.get("CPPDEFINES", []):
        if isinstance(define, tuple) or isinstance(define, list):
            if define[0] == "APP_VERSION":
                # Oczyszczamy stringa ze zbędnych cudzysłowów (np. \"100.100\" -> 100.100)
                app_version = str(define[1]).replace('\\', '').replace('"', '')
        elif isinstance(define, str):
            if define in ["ESP8266_WATCHDOG", "ESP8266_SATELLITE"]:
                device_type = define

    # Zbudowanie ścieżki do folderu docelowego np. "firmware/ESP8266_WATCHDOG"
    output_dir = os.path.join(project_dir, "firmware", device_type)
    
    # Utworzenie folderu, jeśli jeszcze nie istnieje
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Ustalenie docelowej nazwy pliku
    file_name = f"{device_type}_firmware_v.{app_version}.bin"
    output_file = os.path.join(output_dir, file_name)

    # Kopiowanie gotowego pliku
    print(f"\n=======================================================")
    print(f" Zapisano gotowy Firmware do:")
    print(f" {output_file}")
    print(f"=======================================================\n")
    shutil.copy(target_bin, output_file)

# Zarejestrowanie naszej funkcji, aby wykonała się na samym końcu budowania
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", after_build)