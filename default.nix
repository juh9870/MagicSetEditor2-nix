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
  '';

  meta = with pkgs.lib; {
    description = "Magic Set Editor";
    license = licenses.gpl2Only;
  };
}
