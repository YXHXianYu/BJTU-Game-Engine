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
    rust-overlay.url = "github:oxalica/rust-overlay";
  };

  outputs = { self, nixpkgs, flake-utils, rust-overlay, nur, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        overlays = [ (import rust-overlay) (nur.overlay) ];
        pkgs = import nixpkgs {
          inherit system overlays;
        };
        rust-tools = pkgs.rust-bin.nightly.latest.default.override {
          extensions = [ "rust-src" ];
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
            clang-tools
            clang
            llvmPackages_17.bintools
            pkg-config
            cmake
            python3
          ] ++ [
            rust-tools
          ] ++ (with pkgs.darwin.apple_sdk.frameworks; pkgs.lib.optionals pkgs.stdenv.isDarwin [
          ]);
        };
      }
    );
}
