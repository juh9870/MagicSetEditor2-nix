{ pkgs ? import <nixpkgs> { } }:

pkgs.stdenv.mkDerivation rec {
  name = "magic-set-editor";

  src = ./.;

  nativeBuildInputs = with pkgs; [ cmake pkg-config wrapGAppsHook ];
  buildInputs = with pkgs; [ gpp boost wxGTK32 hunspell ];

  buildPhase = ''
    cmake . -DCMAKE_BUILD_TYPE=Release
    cmake --build .
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp ./magicseteditor $out/bin/

    # Install the icon
    mkdir -p $out/share/icons/hicolor/256x256/apps
    cp ./resource/icon/app.ico $out/share/icons/hicolor/256x256/apps/magicseteditor.ico

    # Install the desktop file
    mkdir -p $out/share/applications
    cp ./magicseteditor.desktop $out/share/applications/
  '';

  meta = with pkgs.lib; {
    description = "Magic Set Editor";
    license = licenses.gpl2Only;
  };
}
