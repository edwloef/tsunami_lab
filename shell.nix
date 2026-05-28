{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = with pkgs; [
        scons
        netcdf
        nlohmann_json
    ]
}
