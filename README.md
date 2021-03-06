# ModBringer
ModBringer a Mod Loader for the game ScourgeBringer by Flying Oak Games. Check out the rules for modding ScourgeBringer here: https://steamcommunity.com/games/1037020/announcements/detail/3012317192167588072

ModBringer provides an interface for managing community made Skins/Texture and Game Catalog files. 

The skins below are compatible with ScourgeBringer v1.52

Included Skins:
- Rel (the character from the ScourgeBringer prequel comicbook)
- Yung Venus aka YV (from the game Nuclear Throne by Vlambeer)
- Kyhra-Swordless: https://github.com/camoweed/Scourgebringer-Skins/releases/tag/swordless

Community Skins:
- Youmu Konpaku (Touhou): https://github.com/GeneralWziumKenobi/Scourgebringer-Youmu-Skin
- Crewmate(?) (Among Us): https://github.com/MaximusG29/SusBringer/releases/tag/1.0

Tileset Texturepack
- Touhou Tileset pack - https://github.com/GeneralWziumKenobi/Scourgebringer-Touhou-Tileset-Pack

(In order to include a community skin in ModBringer, make a folder for the skin next to the other skins and include the skin in that folder. You will make one folder per skin.)

ModBringer is currently only compatible with Windows, but it is planned to be compatible with other operating systems.

# Usage
1. Download the latest release (e.g Windowsx86-ModBringer-Release.zip) https://github.com/Squiblydoo/ModBringer/releases/
2. Unzip the files.
3. Run "Modbringer.exe"
4. Select Setup option.
5. Select desired skin option.
6. Launch game and use selected skin.

If at any time, you have unexpected difficulty loading skins or the game, please use Steam to verify game files and restore game files. 
ModBringer backs up the default skins for safe keeping, but something could still go wrong.

# Adding your own modifications (Advanced Users)
If you want to add your own skin to the launcher for personal use, follow this procedure.
1. Create a folder in the "Skins" folder in the "Mod" folder next to where you saved ModBringer.exe.
2. Put your an "xnb" file named identically to the Sprite sheet you are replacing.
3. Run ModBringer option 2 "Setup/Update". This will copy the files where they need to go.
4. Your new skin should now appear in the propper ModBringer menu.

# ModBringer 1.1 Update
ModBringer now backs up the appropriate files for all modding categories and handles all modding categories.
ModBringer now has maintenance options.

Minor Update: I updated the skins in the Repository but not in the release. These updated skins are identical graphically, but they use MGCB (MonoGame Content Builder) for compressing the XNB for use by the game. We had previously been using https://github.com/LeonBlade/xnbcli, but the XNB files were usually 16K KB. With MGCB they are ~200 KB in size.

# Contributing
If you would like to contribute to ModBringer with content such as Skins and Mods to the offical releases, please issue a request through Github, join the ScourgeBringer Discord, or contact me (Squiblydoo).

If you have problems, stop by the ScourgeBringer discord and let me know or place an issue or pull request.

# License
BSD-3

# Credit
The work on the Rel, KyhraSwordless and YV skins belong to camoweed_ from the ScourgeBringer Discord.
