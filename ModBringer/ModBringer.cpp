// ModBringer.cpp 
#
#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>


using namespace std;
// TO DO: Make this path Relative and OS agnositic and drive agnostic
// Alternatively, Save path to file if user has to manually specify directory
// At Some point, all paths should really become Relative
filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };

enum class Options {
    Exit,
    Setup,
    Modifications,
    Maintenance,
    Help
};

struct Settings {
    filesystem::path gameDirectory;
    string currentSkin = "";
    string tileset = "";
};


bool verifyDirectory(filesystem::path directory) {
    try {
        directory.make_preferred();
        bool result;
        (!exists(directory)) ? result = false : result = true;
        return result;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what();
        return false;
    }
}
/*
void checkSetting(const string str) {
    
};

void writeSetting(string str) {
    
};
*/

int main()
{
    cout <<
        " _______  _______  ______   ______   _______ _________ _        _______  _______  _______ \n"
        "(       )(  ___  )(  __  \\ (  ___ \\ (  ____ )\\__   __/( (    /|(  ____ \\(  ____ \\(  ____ )\n"
        "| () () || (   ) || (  \\  )| (   ) )| (    )|   ) (   |  \\  ( || (    \\/| (    \\/| (    )|\n"
        "| || || || |   | || |   ) || (__/ / | (____)|   | |   |   \\ | || |      | (__    | (____)|\n"
        "| |(_)| || |   | || |   | ||  __ (  |     __)   | |   | (\\ \\) || | ____ |  __)   |     __)\n"
        "| |   | || |   | || |   ) || (  \\ \\ | (\\ (      | |   | | \\   || | \\_  )| (      | (\\ (   \n"
        "| )   ( || (___) || (__/  )| )___) )| ) \\ \\_____) (___| )  \\  || (___) || (____/\\| ) \\ \\__\n"
        "|/     \\|(_______)(______/ |/ \\___/ |/   \\__/\\_______/|/    )_)(_______)(_______/|/   \\__/\n"
        "by Squiblydoo\n" << endl;
                                                                                          
    Options optionNumber = Options::Exit;
    int selection;
    Settings settings;


    // Check for existing configuration file; if it exists, read in the current settings
    // TO DO:
    // Check Plausibility of creating a config file using Boost::property_tree and then read and write to tree as needed

    const auto modBringer_path = filesystem::current_path();
    /*
    if (exists(modBringer_path / "ModBringer.config")) {
        ifstream config{ "ModBringer.config" };
        cout << "Current Settings: \n";
        std::string str;
        while (std::getline(config, str)) {
            std::cout << str << "\n";
        };
        cout << endl;
    }
    else {
        ofstream config{ "ModBringer.config", ios::out | ios::app };
        config << "GameDirectory:\n";
        settings.currentSkin = "DefaultSkin";
        config << "Skin: " << settings.currentSkin << "\n";
        settings.tileset = "DefaultTileset";
        config << "Tileset: " << settings.tileset << "\n";
    };
    */

    while (true) {
        cout << "Type number to section an option.\nOptions\n"
            "[0] Exit\n"
            "[1] Setup or Update\n"
            "[2] Select Modifications \n"
            "[3] ModBringer Maintenance\n"
            "[4] Help" << endl;

        cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            cout << "Option 0 selected\nExiting." << endl;
            std::exit(0);
            break;

        } case Options::Setup: {
            cout << "Option 1 selected.\n";


            //cout << "Creating/Updating Mod Directory: C:/Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\\Content\\Mod\n\n";
            filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };
            filesystem::path contentFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" };
            const auto modFiles = modBringer_path / "Mods";
           
            if (!verifyDirectory(contentFolder)) {
                cout << "Game files could not be found.\nIf the game is installed, please specify the game directory: \n";
                
                // TO DO: Logic for specifing game directory

                
                // Exit back to menu if unable to find game directory.
                continue;
                
            };

            // If the mod directory exists, we just need to update it with the items in the directory next to ModBringer.
            const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;
            if (exists(modFiles) && exists(contentFolder)) {
                filesystem::copy(modFiles, modFolder, copyOptions);
            }


            // Ensure backups of original skins exist
            const auto skinDirectory = modFolder / "Skins";
            const auto backupDirectory = skinDirectory / "Default Skin";


            // Make backups if the Backup directory does not exist.
            if (!verifyDirectory(backupDirectory)) {

                // Creates Backup Directory:
                filesystem::create_directory(backupDirectory);

                // Creates Backups of "common_X.xnb" files

                const auto skinDirectory = modFolder / "Skins";
                const auto backupSkinDirectory = skinDirectory / "Default Skin";
                for (auto entry : filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && entry.path().has_extension()) {
                        filesystem::copy(entry, backupSkinDirectory);
                    };
                };


                // Creates backups of Catalog files
                const auto generalBackup = modFolder / "Backups";
                const auto gameplayMods = modFolder / "GameplayMods"; 
                const auto gameplayModsBackup = generalBackup / "GameplayModsBackup";
                const auto tilesetDirectory = modFolder / "Tilesets";
                const auto tilesetBackupDirectory = generalBackup / "TilesetBackup";
                const auto otherTextureMods = modFolder / "OtherTextures";
                const auto otherTextureBackups = generalBackup / "OtherTextures";
                const auto languageFileMods = modFolder / "LanguageFiles";
                const auto languageFileBackups = generalBackup / "LanguageFiles";


                if (!exists(generalBackup)) filesystem::create_directory(generalBackup);
                // TODO:
                // Find out if this is a stupid way to do this. It seems to work at the moment.
                filesystem::path saveDirectory = filesystem::temp_directory_path();
                filesystem::path modBringerDirectory = filesystem::current_path();
                filesystem::current_path(saveDirectory);
                filesystem::current_path("../../LocalLow/Flying\ Oak\ Games/ScourgeBringer");
                std::cout << "Current Directory: " << filesystem::current_path();
                filesystem::copy(filesystem::current_path() / "0.sav", generalBackup);
                filesystem::current_path(modBringerDirectory);


                // Backup Catalog files
                if (!exists(gameplayModsBackup)) {
                    filesystem::create_directory(gameplayMods);
                    filesystem::create_directory(gameplayModsBackup);

                    //  Catalog files don't have an extension but are a regular file
                    for (auto entry : filesystem::directory_iterator(contentFolder)) {
                        if (entry.is_regular_file() && !entry.path().has_extension()) {
                            filesystem::copy(entry, gameplayModsBackup);
                        }
                    };
                }

                // Backup Tilesets
                if (!exists(tilesetBackupDirectory)) {
                    filesystem::create_directory(tilesetBackupDirectory);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Tilesets")) {
                        filesystem::copy(entry, tilesetBackupDirectory);
                    }

                }

                // Backup various texture files
                if (!exists(otherTextureBackups)) {
                    const auto backupCopyOptions = filesystem::copy_options::recursive;
                    filesystem::create_directory(tilesetDirectory);
                    const auto bossBackup = generalBackup / "Bosses";
                    filesystem::copy(contentFolder / "Bosses", bossBackup, copyOptions);
                    const auto doorBackup = generalBackup / "Doors";
                    filesystem::copy(contentFolder / "Doors", doorBackup, copyOptions);
                    const auto backgroundBackup = generalBackup / "Backgrounds";
                    filesystem::copy(contentFolder / "Backgrounds", backgroundBackup, copyOptions);
                    

                }

                //Backup Language files
                if (!exists(languageFileBackups)) {
                    filesystem::create_directory(languageFileBackups);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Localizations")) {
                        filesystem::copy(entry, languageFileBackups);
                    }

                }

            }



            break;

        } case Options::Modifications: {
            cout << "Option 2 selected.\n\n";
            if (!verifyDirectory(modFolder)) {
                cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << endl;
                break;
            };
            if (exists(modFolder)) {
                int entryNum{ 0 };
                int modSelection;

                std::map<int, string> modTypes;
                int typeNum{ 0 };
                for (const auto entry : filesystem::directory_iterator(modFolder)) {
                    cout << "[" << typeNum << "] ";
                    cout << entry.path().filename().string() << "\n";
                    modTypes.emplace(typeNum, entry.path().filename().string());
                    typeNum++;
                }

                cout << "Please make a selection.\n";
                cin >> modSelection ;
                try {
                    cout << modTypes.at(modSelection) << "\n";

                    if (modTypes.at(modSelection) == "GameplayMods") {
                        auto gameplayModDirectory = modFolder / "GameplayMods";
                        std::map<int, string> gameplayMods;
                        int gameplayModNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(gameplayModDirectory)) {
                            cout << "[" << gameplayModNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            gameplayMods.emplace(gameplayModNum, entry.path().filename().string());
                            gameplayModNum++;
                        }

                        if (gameplayModNum == 0) {
                            cout << "There are no Gameplay mods available." << endl;
                        }

                        cout << "[" << gameplayModNum << "] Cancel\n";
                        int gameplayModSelection;
                        cout << "Please make a selection." << endl;
                        cin >> gameplayModSelection;
                        try {
                            gameplayMods.at(gameplayModSelection);


                        }
                        catch (std::exception) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };


                    //Handle Tileset Selection
                    if (modTypes.at(modSelection) == "Tilesets") {
                        auto tilesetDirectory = modFolder / "Tilesets";
                        std::map<int, string> tilesets;
                        int tilesetNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(tilesetDirectory)) {
                            cout << "[" << tilesetNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            tilesets.emplace(tilesetNum, entry.path().filename().string());
                            tilesetNum++;
                        }
                        
                        if (tilesetNum == 0) {
                            cout << "There are no tileset mods available." << endl;
                        }

                        cout << "[" << tilesetNum << "] Cancel\n";
                        int tilesetSelection;
                        cout << "Please make a selection." << endl;
                        cin >> tilesetSelection;
                        try {
                            tilesets.at(tilesetSelection);
                            

                        }
                        catch (std::exception) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };

                    // Handle Skin Selection
                    if (modTypes.at(modSelection) == "Skins") {


                        const auto skinDirectory = modFolder / "Skins";
                        cout << "\nSkin options available: \n";
                        std::map<int, string> skins;
                        int skinNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(skinDirectory)) {
                            cout << "[" << skinNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            skins.emplace(skinNum, entry.path().filename().string());
                            skinNum++;
                        }

                        cout << "[" << skinNum << "] Cancel\n" ;

                        int skinSelection;
                        cout << "Please make a selection." << endl;
                        cin >> skinSelection;
                        try {
                            skins.at(skinSelection);
                            cout << "You want to replace the current skin with " << skins.at(skinSelection) << "?\n";
                            cout << "[y/n]: ";
                            string answer;
                            cin >> answer;
                            if (answer == "y") {
                                cout << "Copying skin.\n";

                                const auto replaceOptions = filesystem::copy_options::overwrite_existing;
                                auto selectedSkinDirectory = skinDirectory / skins.at(skinSelection);
                                for (const auto& entry : filesystem::directory_iterator{ selectedSkinDirectory }) {
                                    const auto file = filesystem::path(selectedSkinDirectory / entry);
                                    if (file.extension() == ".xnb") {
                                        filesystem::copy(file, modFolder/ "..", replaceOptions);
                                        cout << "Copying: " << file.filename().string() << "\n";
                                    }
                                };
                            }
                            else {
                                cout << "Canceled.\nReturning to main menu.\n\n ";
                            }
                        }
                        catch (std::exception&) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }
                    }
                
                
                
                }
                catch (std::exception& e) {
                    cout << "Exception: " << e.what();
                }

            }
            break;

        } case Options::Maintenance : {
            cout << "Option 3 selected\n";

                if (!verifyDirectory(modFolder)) {
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << endl;
                    break;
                }

                cout << "[0] Return to main menu\n" 
                << "[1] Delete Mod Folder\n"
            << "Please make a selection.\n";
                int maintenanceSelection{};

            cin >> maintenanceSelection;
            if (maintenanceSelection == 1) {
                cout << "You want to remove the Mod Folder and all mods?\n";
                cout << "[y/n]: ";
                string answer;
                cin >> answer;
                if (answer == "y") {
                    // TO DO:
                    // Ask the user if they want all backups put back in place
                    // This will include their Save File  
                    cout << "Removing Mod Folder.\n To use Mods again, run Setup from the main menu." << endl;
                    filesystem::remove_all(modFolder);
                }
                else {
                    cout << "Canceled.\nReturning to main menu.\n\n ";
                }
            }

            
            break;
        
        } case Options::Help : {
            cout << "Option 4 selected\n";
            cout << "Help topics:\n"
                << "[0] Return to main menu\n"
                << "[1] About ModBringer\n" 
                << "[2] How to Add New Mods\n"
                << "Please make a selection:" << endl;
            int helpSelection{};

            cin >> helpSelection;

            switch (helpSelection) {
            case (0): { // Return to Main Menu
                
                continue;

            } case (1): {
                // TO DO: Provide useful information here.

                break;
            } default: {
                // Invalid section made
                break;
            }
            }

            break;

        }default: {
            break;
        }
        }
    };
}
