// ModBringer.cpp 
#
#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono> // Used for identifying file timestamps
#include <stdio.h> // Used for identifying OS 
#include <limits>  // Used by the Pause function
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

// MSVC complains about std::getenv right now we are disabling the warning
// std::getenv is only used when the OS is Macintosh
#pragma warning(disable : 4996) 

enum class Options {
    Setup,
    Modifications,
    Maintenance,
    Help,
    Exit
};

struct Settings {
    std::filesystem::path gameDirectory;
    std::filesystem::path contentDirectory;
    std::string operatingSystem{};
};

bool verifyDirectory(std::filesystem::path directory);

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory, std::string extension);
void restoreAll(std::filesystem::path contentDirectory);
int getSelection();
//void handleModSelection(std::filesystem::path contentPath, std::filesystem::path activeFilePath, std::filesystem::path backupDirectory, std::string fileType, std::map<int, std::string> map, int userSelection);
void pause();

std::string ansi(const wchar_t* wbuf, int codepage = 1254)
{
    int len = WideCharToMultiByte(codepage, 0, wbuf, -1, 0, 0, 0, 0);
    std::string shortname;
    shortname.resize(len, L'\0');
    WideCharToMultiByte(codepage, 0, wbuf, -1, &shortname[0], len, 0, 0);
    shortname.resize(len - 1);
    return shortname;
}


int main()
{
    int codepage = 1254;
    SetConsoleOutputCP(codepage);
    SetConsoleCP(codepage);
    std::cout <<
        "_________          _______  _        _______  _______  _______  _______ __________________ _______  _______  _       \n"
        "\\__   __/|\\     /|(  ____ )| \\    /\\(  ____ \\(  ____ \\(  ____ \\(  ____ \\\\__   __/\\__   __/(  ____ )(  ____ \\( (    /|\n"
        "   ) (   | )   ( || (    )||  \\  / /| (    \\/| (    \\/| (    \\/| (    \\/   ) (      ) (   | (    )|| (    \\/|  \\  ( |\n"
        "   | |   | |   | || (____)||  (_/ / | |      | (__    | |      | (__       | |      | |   | (____)|| (__    |   \\ | |\n"
        "   | |   | |   | ||     __)|   _ (  | |      |  __)   | | ____ |  __)      | |      | |   |     __)|  __)   | (\\ \\) |\n"
        "   | |   | |   | || (\\ (   |  ( \\ \\ | |      | (      | | \\_  )| (         | |      | |   | (\\ (   | (      | | \\   |\n"
        "   | |   | (___) || ) \\ \\__|  /  \\ \\| (____/\\| (____/\\| (___) || (____/\\   | |   ___) (___| ) \\ \\__| (____/\\| )  \\  |\n"
        "   )_(   (_______)|/   \\__/|_/    \\/(_______/(_______/(_______)(_______/   )_(   \\_______/|/   \\__/(_______/|/    )_)\n";
    std::cout << ansi(L"Squiblydoo tarafından yapılmıştır. Türkçe Çeviri Hugo the Waffle tarafından yapılmıştır.\n") << std::endl;

    // Detect Operating System
    Settings settings;
#ifdef _WIN32 || _WIN64
    settings.operatingSystem = "Windows";
#elif __APPLE__ || __MACH__
    settings.operatingSystem = "Macintosh";
#endif

    int selection;

    // Check for existing configuration file; if it exists, read in the current settings
    const auto modBringer_path = std::filesystem::current_path();

    if (exists(modBringer_path / "ModBringer.config")) {
        std::ifstream config{ "ModBringer.config" };
        std::cout << ansi(L"Geçerli Ayarlar: \n");
        std::string savedGameDirectory;
        std::getline(config, savedGameDirectory);
        savedGameDirectory.erase(remove(savedGameDirectory.begin(), savedGameDirectory.end(), '\"'), savedGameDirectory.end());
        settings.gameDirectory = savedGameDirectory;
        settings.contentDirectory = settings.gameDirectory / "Content";
        std::cout << ansi(L"Oyun Dizini: ") << savedGameDirectory << "\n";
        std::cout << ansi(L"İşletim Sistemi: ") << settings.operatingSystem;
        std::cout << std::endl;
    }


    while (true) {
        std::cout << ansi(L"\nBir seçeneği seçmek için seçeneğin önündeki numarayı yazın. \nAyarlar:\n"
            L"[0] Kurulum veya Güncelleme\n"
            L"[1] Türkçe Yamayı Kur\n"
            L"[2] Değişiklikleri Geri Al\n"
            L"[3] Yardım\n"
            L"[4] Çık\n") << std::endl;

        std::cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            std::cout << ansi(L"\nÇıkılıyor.") << std::endl;
            std::exit(0);

        } case Options::Setup: {
            if (!verifyDirectory(settings.gameDirectory)) { // Game directory not set

                //Check common places first; if they fail, ask the user.
                if (settings.operatingSystem == "Windows") {

                    auto defaultWindowsPath = std::filesystem::path{ R"(C:\Program Files (x86)\Steam\steamapps\common\ScourgeBringer\)" };
                    if (verifyDirectory(defaultWindowsPath)) {
                        std::filesystem::path contentFolder = defaultWindowsPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultWindowsPath << std::endl;
                        settings.gameDirectory = defaultWindowsPath;
                        settings.contentDirectory = contentFolder;
                        settings.operatingSystem = "Windows";
                    }
                    else {
                        std::string userProvidedGameDirectory;
                        std::filesystem::path userProvidedPath;

                        do {
                            std::cout << ansi(L"Oyun dosyaları bulunamadı.\nEğer oyun yüklüyse, lütfen oyun dizinini belirtiniz:\nÖrnek:\nC:\\Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\n");
                            std::cin.ignore(100, '\n');
                            getline(std::cin, userProvidedGameDirectory);
                            userProvidedPath = userProvidedGameDirectory;
                        } while (!verifyDirectory(userProvidedPath));
                        std::filesystem::path contentFolder = userProvidedPath;
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << contentFolder << std::endl;
                        settings.gameDirectory = userProvidedPath;
                        settings.contentDirectory = contentFolder;
                        std::cout << ansi(L"Oyun dizini kaydediliyor\n");
                    };
                }
                else if (settings.operatingSystem == "Macintosh") {

                    auto home = std::getenv("HOME");
                    auto directory = std::filesystem::path{ home };
                    std::filesystem::path defaultMacPath = directory / "Library/Application Support/Steam/steamapps/common/ScourgeBringer/ScourgeBringer.app/Contents/Resources/";

                    if (verifyDirectory(defaultMacPath)) {
                        std::filesystem::path contentFolder = defaultMacPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultMacPath << std::endl;
                        settings.gameDirectory = defaultMacPath;
                        settings.contentDirectory = contentFolder;
                        settings.operatingSystem = "Macintosh";

                    }

                }



            }
            const auto contentFolder = settings.contentDirectory;
            const auto modFolder = contentFolder / "Mods";
            const auto modFiles = modBringer_path / "Mods";


            // If the mod directory exists, we just need to update it with the items in the directory next to ModBringer.
            const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
            try {
                std::filesystem::copy(modFiles, modFolder, copyOptions);
            }
            catch (std::exception& e) {
                std::cout << ansi(L"Belki mod klasörü mevcut değildir veya buraya yazmak için uygun izinlere sahip değilsinizdir: ") << e.what();
            }


            // Setup for backing up original gamefiles
            const auto moddedSkins = modFolder / "Skins";
            const auto defaultSkins = moddedSkins / "Default Skin";
            const auto generalBackup = modFolder / "Backups";
            const auto gameplayMods = modFolder / "Gameplay Mods";
            const auto defaultGameplay = gameplayMods / "Default Gameplay";
            const auto tilesetDirectory = modFolder / "Tileset Mods";
            const auto defaultTilesets = tilesetDirectory / "Default Tilesets";
            const auto bossMods = modFolder / "Boss Mod Images";
            const auto defaultBosses = bossMods / "Default Bossess";
            const auto languageFileMods = modFolder / "Language File Mods";
            const auto defaultLanguageFiles = languageFileMods / "Default Language Files";
            const auto miscImageMods = modFolder / "Misc Images Mods";
            const auto defaultMiscImages = miscImageMods / "Default Misc Images";
            const auto backgroundMods = modFolder / "Background Mods";
            const auto defaultBackgrounds = backgroundMods / "Default Backgrounds";
            const auto modPackFolder = modFolder / "Mod Packs";
            const auto moddedFonts = modFolder / "Modded Fonts";
            const auto defaultFonts = moddedFonts / "Default Fonts";


            // Creates BackupSkin Directory:
                 // Creates Backups of "common_X.xnb" files
            if (!verifyDirectory(defaultSkins)) {
                if (!verifyDirectory(moddedSkins)) {
                    std::filesystem::create_directory(moddedSkins);
                }
                std::filesystem::create_directory(defaultSkins);
                for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && entry.path().has_extension()) {
                        std::filesystem::copy(entry, defaultSkins);
                    };
                };
            }

            // Backup Catalog files
            if (!exists(defaultGameplay)) {
                std::filesystem::create_directory(gameplayMods);
                std::filesystem::create_directory(defaultGameplay);

                //  Catalog files don't have an extension but are a regular file
                for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && !entry.path().has_extension()) {
                        std::filesystem::copy(entry, defaultGameplay);
                    }
                };
                //std::cout << "Backups of Gameplay files created." << std::endl;
            }

            // Backup Tilesets
            if (!exists(defaultTilesets)) {
                std::filesystem::create_directory(tilesetDirectory);
                std::filesystem::create_directory(defaultTilesets);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Tilesets")) {
                    std::filesystem::copy(entry, defaultTilesets);
                }
                //std::cout << "Backups of Tilesets created." << std::endl;
            }

            // Backup Boss Image Files
            if (!exists(defaultBosses)) {
                std::filesystem::create_directory(bossMods);
                std::filesystem::create_directory(defaultBosses);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Bosses")) {
                    std::filesystem::copy(entry, defaultBosses);
                }
                //std::cout << "Backups of Boss sprites created." << std::endl;
            }

            // Backup Localization files
            if (!exists(defaultLanguageFiles)) {
                std::filesystem::create_directory(languageFileMods);
                std::filesystem::create_directory(defaultLanguageFiles);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Localizations")) {
                    std::filesystem::copy(entry, defaultLanguageFiles);
                }
                //std::cout << "Backups of Localization files created." << std::endl;
            }

            // Backup Misc Files
            if (!exists(defaultMiscImages)) {
                std::filesystem::create_directory(miscImageMods);
                std::filesystem::create_directory(defaultMiscImages);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Doors")) {
                    std::filesystem::copy(entry, defaultMiscImages);
                }
                //std::cout << "Backups of Misc Sprites created." << std::endl;
            }

            // Backup background images
            if (!exists(defaultBackgrounds)) {
                std::filesystem::create_directory(backgroundMods);
                std::filesystem::create_directory(defaultBackgrounds);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Backgrounds")) {
                    std::filesystem::copy(entry, defaultBackgrounds);
                }
                //std::cout << "Backups of Background Sprites created." << std::endl;
            }

            // Backup font files
            if (!exists(defaultFonts)) {
                std::filesystem::create_directory(moddedFonts);
                std::filesystem::create_directory(defaultFonts);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Fonts")) {
                    std::filesystem::copy(entry, defaultFonts);
                }
                //std::cout << "Backups of Background Sprites created." << std::endl;
            }


            if (!exists(modPackFolder)) {
                std::filesystem::create_directory(modPackFolder);
            }


            // Make backup of player save
            // TODO: Rename save in order to make a timestamped backup each time setup is ran
            if (!exists(generalBackup)) std::filesystem::create_directory(generalBackup);
            const auto backupCopyOptions = std::filesystem::copy_options::update_existing;

            std::filesystem::path modBringerDirectory = std::filesystem::current_path();

            // Both of these account for the save being in a User directory and copy the file by changing the working directory to that directory.
            if (settings.operatingSystem == "Windows") {
                std::filesystem::path tempDirectoryForSave = std::filesystem::temp_directory_path();
                std::filesystem::current_path(tempDirectoryForSave);
                std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup, backupCopyOptions);

                //Rename save with timestamp.
                std::stringstream saveTimeStamp{};
                const time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                saveTimeStamp << std::put_time(std::localtime(&timenow), "%F-%H%M");
                std::string timeStampString = "0.sav" + saveTimeStamp.str();
                std::filesystem::rename(generalBackup / "0.sav", generalBackup / timeStampString);

                std::cout << ansi(L"ScourgeBringer kayıt dosyasının yedeği şu adla kaydedildi: ") << timeStampString << std::endl;
                std::cout << ansi(L"Kurulum tamamlandı.") << std::endl;
            }
            else if (settings.operatingSystem == "Macintosh") {
                auto home = getenv("HOME");
                auto directory = std::filesystem::path{ home };
                std::filesystem::path tempDirectoryForSave = directory / "Library/Application Support/Flying Oak Games/ScourgeBringer";
                std::filesystem::current_path(tempDirectoryForSave);
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup, backupCopyOptions);

                //Rename save with timestamp.
                std::stringstream saveTimeStamp{};
                const time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                saveTimeStamp << std::put_time(std::localtime(&timenow), "%F-%H%M");
                std::string timeStampString = "0.sav" + saveTimeStamp.str();
                std::filesystem::rename(generalBackup / "0.sav", generalBackup / timeStampString);

                std::cout << ansi(L"ScourgeBringer kayıt dosyasının yedeği şu adla kaydedildi: ") << timeStampString << std::endl;
                std::cout << ansi(L"Kurulum tamamlandı.") << std::endl;
            }
            // Return working directory to previous directory
            std::filesystem::current_path(modBringerDirectory);

            continue;

        } case Options::Modifications: {
            //std::cout << "Option " << selection << " selected\n";
            if (settings.contentDirectory == "" || !verifyDirectory(settings.contentDirectory / "Mods")) {
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << ansi(L"!!!!!!!Mod dizini yok. Lütfen ilk önce kurulumu yapınız.!!!!!!!\n")
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << std::endl;
                continue;
            };
            if (exists(settings.contentDirectory / "Mods")) {
                auto selectedModDirectory = settings.contentDirectory / "Mods" / "Mod Packs" / "Turkish Translation";
                auto selectedMod = selectedModDirectory / "Turkish Translation";
                const auto replaceOptions = std::filesystem::copy_options::overwrite_existing;
                try {

                    restoreAll(settings.contentDirectory);

                    /*
                    If the user wants to restore defaults, we will perform the restoreAll function and do nothing else.
                    Before changing Mod Packs, we will always restore all other changes to the original settings.
                    */
                    for (auto& entry : std::filesystem::directory_iterator{ selectedModDirectory }) {
                        std::string fileStart = entry.path().filename().string().substr(0, 3);
                        /*
                        Mod Packs are different mod types, so we will read some of the file name
                        and then move them to the appropriate directory.
                        */
                        // Handle Gameplay files
                        if (fileStart == "Bos" ||
                            fileStart == "Bul" ||
                            fileStart == "Emi" ||
                            fileStart == "Ene" ||
                            fileStart == "Map" ||
                            fileStart == "Par" ||
                            fileStart == "Roo"
                            ) {
                            std::filesystem::copy(entry, settings.contentDirectory, replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }
                        // Handle Skins
                        if (fileStart == "com") {
                            std::filesystem::copy(entry, settings.contentDirectory, replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }
                        //Handle Tilesets
                        if (fileStart == "til") {
                            std::filesystem::copy(entry, settings.contentDirectory / "Tilesets", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }
                        // Handle Misc Images/Doors
                        if (fileStart == "doo") {
                            std::filesystem::copy(entry, settings.contentDirectory / "Doors", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }
                        // Handle Boss images
                        if (fileStart == "bos") {
                            std::filesystem::copy(entry, settings.contentDirectory / "Bosses", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }
                        // Handle Backgrounds
                        if (fileStart == "bac") {
                            std::filesystem::copy(entry, settings.contentDirectory / "Backgrounds", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";

                        }
                        // Account for font files
                        if (fileStart == "pix" ||
                            fileStart == "hig"
                            ) {
                            std::filesystem::copy(entry, settings.contentDirectory / "Fonts", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }

                        // Account for localization mods. The names all differ but are only 2 char in length
                        if (entry.path().filename().string().length() == 2) {
                            std::filesystem::copy(entry, settings.contentDirectory / "Localizations", replaceOptions);
                            //std::cout << "Copying: " << entry.path().filename().string() << "\n";
                        }



                    }
                }
                catch (std::exception e) {
                    std::cout << "Exception: " << e.what();
                }

            }
            std::cout << ansi(L"Kurulum tamamlandı.") << std::endl;
            continue;

        } case Options::Maintenance: {

            if (settings.contentDirectory == "" || !verifyDirectory(settings.contentDirectory / "Mods")) {
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << ansi(L"!!!!!!!Mod dizini yok. Lütfen ilk önce kurulumu yapınız.!!!!!!!\n")
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << std::endl;
                continue;
            }

            std::cout << ansi(L"[0] Mod klasörünü sil\n")
                << ansi(L"[1] Yedek kayıt dosyasını geri yükle\n")
                << ansi(L"[2] Varsayılan oyunu geri yükle\n")
                << ansi(L"[3] Başka bir oyun dizini belirt\n")
                << ansi(L"[4] İptal\n")
                << std::endl;
            int maintenanceSelection = getSelection();

            switch (maintenanceSelection) {
            case (0): {
                std::cout << ansi(L"Mod klasörünü ve tüm modları silmek mi istiyorsunuz?\nBu seçim aynı zamanda tüm oyun dosyalarını önceki orijinal durumlarıyla değiştirecektir. (e=evet, h=hayır)\n");
                std::cout << "[e/h]: ";
                std::string answer;
                std::cin >> answer;
                if (answer == "e") {
                    restoreAll(settings.contentDirectory);
                    std::cout << ansi(L"Mod klasörü siliniyor.\n Tekrar mod kullanmak için ana menüdeki Kurulum seçeneğini seçin.") << std::endl;
                    std::filesystem::remove_all(settings.contentDirectory / "Mods");
                    std::cout << ansi(L"Oyun dosyaları geri yükleniyor.\n");
                }
                else {
                    std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n ");
                }
                continue;
            }
            case (1): { // Restore Save file
                std::cout << ansi(L"Bunu yalnızca kayıt dosyanız bozuksa ve mevcut kayıt dosyanızın üzerine yazmanız gerekiyorsa kullanın.\n"
                    L"Bu pek olası olmasa da, bazı modlar kayıt dosyanızla uyumlu değilse gerçekleşebilir. Türkçe yama böyle bir şey yapmamalı.\n"
                    L"Bir kayıt dosyasını geri getirmek mi istiyorsun? (e=evet, h=hayır)\n");
                std::cout << "[e/h]: ";
                std::string answer;
                std::cin >> answer;
                if (answer == "e") {
                    std::cout << ansi(L"Lütfen bir kayıt dosyası seçin:\n");

                    std::filesystem::path generalBackup = settings.contentDirectory / "Mods" / "Backups";
                    std::map<int, std::string> backups;
                    int backupNumber{ 0 };
                    for (auto entry : std::filesystem::directory_iterator(generalBackup)) {
                        std::cout << "[" << backupNumber << "] ";
                        std::cout << entry.path().filename().string() << "\n";
                        backups.emplace(backupNumber, entry.path().filename().string());
                        backupNumber++;
                    }
                    std::cout << "[" << backupNumber << "] " << ansi(L"İptal\n");

                    int backupSelection = getSelection();

                    std::filesystem::path modBringerDirectory = std::filesystem::current_path();
                    auto replaceOptions = std::filesystem::copy_options::overwrite_existing;

                    if (settings.operatingSystem == "Windows") {
                        std::filesystem::path saveDirectory = std::filesystem::temp_directory_path();
                        std::filesystem::current_path(saveDirectory);
                        std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                    }
                    else if (settings.operatingSystem == "Macintosh") {
                        auto home = getenv("HOME");
                        auto directory = std::filesystem::path{ home };
                        std::filesystem::path saveDirectory = directory / "Library/Application Support/Flying Oak Games/ScourgeBringer";
                        std::filesystem::current_path(saveDirectory);
                    }
                    try {

                        if (exists(generalBackup / backups.at(backupSelection)) && exists(std::filesystem::current_path() / "0.sav")) {
                            std::filesystem::copy(generalBackup / backups.at(backupSelection), std::filesystem::current_path() / "0.sav", replaceOptions);
                            std::filesystem::current_path(modBringerDirectory);
                            std::cout << "Backup " << backups.at(backupSelection) << " restored.\n";
                        }
                    }
                    catch (std::exception e) {
                        std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n") << std::endl;
                    }
                }
                else {
                    std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n ");
                }
                continue;
            }
            case (2): { // Restore Game Files
                std::cout << ansi(L"Tüm oyun dosyalarını orijinal hallerine geri yüklemek mi istiyorsunuz? (e=evet, h=hayır)\n");
                std::cout << ansi(L"[e/h]: ");
                std::string answer;
                std::cin >> answer;
                if (answer == ansi(L"e")) {
                    restoreAll(settings.contentDirectory);
                }
                else {
                    std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n");
                }
                continue;
            }
            case (3): {
                std::string userProvidedGameDirectory;
                std::filesystem::path userProvidedPath;
                do {
                    std::cout << ansi(L"Lütfen oyunun dizinini belirtiniz:\nÖrnek:\nC:\\Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\n");
                    std::cin.ignore(100, '\n');
                    getline(std::cin, userProvidedGameDirectory);
                    userProvidedPath = userProvidedGameDirectory;
                } while (!verifyDirectory(userProvidedPath));
                std::filesystem::path contentFolder = userProvidedPath;
                std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                config << contentFolder << std::endl;
                settings.gameDirectory = userProvidedPath;
                settings.contentDirectory = contentFolder;
                std::cout << ansi(L"Oyun dizini kaydediliyor\n");
                continue;
            }
            case (4): {
                // User canceled.
                std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n");
                continue;
            }
            default: {

                continue;
            }
            };



            continue;

        } case Options::Help: {
            std::cout << ansi(L"Yardım konuları:\n"
                L"[0] Bu yardımcı program hakkında\n"
                L"[1] İptal\n")
                << std::endl;

            int helpSelection = getSelection();

            switch (helpSelection) {
            case (0): { // About ModBringer

                std::cout << ansi(L"\nBu yardımcı program Squiblydoo tarafından yapılmıştır. Türkçe çeviri Hugo the Waffle tarafından yapılmıştır.\n"
                    L"Eğer daha fazla bilgi almak istiyorsanız, Squiblydoo'ya ScourgeBringer Discord sunucusundan ulaşabilirsiniz veya\n"
                    L"GitHub'da koda bir review bırakabilirsiniz.\n") << std::endl;

                pause();
                continue;
            }

            case (1): { // User canceled.
                std::cout << ansi(L"İptal edildi. Ana menüye dönülüyor. \n");
                continue;
            }
            default: {
                // Invalid section made
                continue;
            }
            }

            continue;

        }default: {
            continue;
        }
        }
    };
}

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory, std::string extension) {
    try {
        const auto copyOptions = std::filesystem::copy_options::overwrite_existing;
        for (auto entry : std::filesystem::directory_iterator(activeDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                const auto backupEntry = backupDirectory / entry.path().filename().string();
                const auto backupTimeStamp = std::filesystem::last_write_time(backupEntry).time_since_epoch();
                //const auto backupTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(backupTimeStamp).count();
                const auto entryTimestamp = std::filesystem::last_write_time(entry).time_since_epoch();
                //const auto entryTimestampMinutes = std::chrono::duration_cast<std::chrono::minutes>(entryTimestamp).count();
                if (entryTimestamp != backupTimeStamp) {
                    std::filesystem::copy(backupEntry, activeDirectory, copyOptions);
                    //std::cout << "---Restoring: " << backupEntry.filename().string() << "\n";
                }
            }
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

}

void restoreAll(std::filesystem::path contentDirectory) {
    //std::cout << "Restoring Gameplay files \n";
    restoreChanged(contentDirectory, contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", "");
    //std::cout << "Restoring Tilesets \n";
    restoreChanged(contentDirectory / "Tilesets", contentDirectory / "Mods" / "Tileset Mods" / "Default Tilesets", ".xnb");
    //std::cout << "Restoring changed Skins\n";
    restoreChanged(contentDirectory, contentDirectory / "Mods" / "Skins" / "Default Skin", ".xnb");
    //std::cout << "Restoring changed backgrounds\n";
    restoreChanged(contentDirectory / "Backgrounds", contentDirectory / "Mods" / "Background Mods" / "Default Backgrounds", ".xnb");
    //std::cout << "Restoring changed Misc Images\n";
    restoreChanged(contentDirectory / "Doors", contentDirectory / "Mods" / "Misc Images Mods" / "Default Misc Images", ".xnb");
    //std::cout << "Restoring changed Language Files\n";
    restoreChanged(contentDirectory / "Localizations", contentDirectory / "Mods" / "Language File Mods" / "Default Language Files", "");
    //std::cout << "Restoring changed Bosses\n";
    restoreChanged(contentDirectory / "Bosses", contentDirectory / "Mods" / "Boss Mod Images" / "Default Bossess", ".xnb");
    //std::cout << "Restoring Fonts\n";
    restoreChanged(contentDirectory / "Fonts", contentDirectory / "Mods" / "Modded Fonts" / "Default Fonts", ".xnb");
}

bool verifyDirectory(std::filesystem::path directory) {
    try {
        directory.make_preferred();
        bool result;
        (!exists(directory)) ? result = false : result = true;
        return result;

    }
    catch (const std::exception& e) {
        std::cerr << ansi(L"Hata: ") << e.what();
        return false;
    }
}

int getSelection() {
    int output;
    std::string input;
    while (true) {
        std::cout << ansi(L"Lütfen bir seçim yapın: ") << std::endl;
        std::cin.ignore(100, '\n');
        std::cin >> input;
        std::stringstream ss(input);

        if (ss >> output && !(ss >> input)) return output;
        // Checks for valid conversion to integer and checks for unconverted input
        std::cin.clear();
        std::cerr << ansi(L"\nGeçersiz girdi. Lütfen tekrar deneyin.\n");

    }
}


void pause()
{
    std::cin.clear();
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string dummy;
    //std::cout << ansi(L"Devam etmek için 'Enter' tuşuna basın . . .");
    std::getline(std::cin, dummy);
}