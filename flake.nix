{
  description = "BJTU-Game-Engine";

  nixConfig = {
    extra-substituters = [
      "https://mirrors.ustc.edu.cn/nix-channels/store"
    ];
    trusted-substituters = [
      "https://mirrors.ustc.edu.cn/nix-channels/store"
    ];
  };


  inputs = {
    nixpkgs.url      = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url  = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      {
        devShells.default = pkgs.mkShell {
          # prioritize system clang, see https://github.com/zed-industries/zed/issues/7036
          # https://github.com/gfx-rs/gfx/issues/2309
          # https://mac.install.guide/commandlinetools/7
          # shellHook = ''
          #   # export PATH=/usr/bin:$PATH
          # '';

          buildInputs = with pkgs; [
            clang
            llvmPackages_16.bintools
            pkg-config
            cmake
            python3
          ] ++ [
          ] ++ (with pkgs.darwin.apple_sdk.frameworks; pkgs.lib.optionals pkgs.stdenv.isDarwin [
          ]);
        };
      }
    );
}
