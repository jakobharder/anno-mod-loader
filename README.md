# Anno Mod Loader

This is a fork of the original mod loader of the great [meow](https://github.com/xforce/anno1800-mod-loader).
Many thanks to him for creating and maintaining that project for so long.

## For Mod Users and Creators

> [!IMPORTANT]
> [Modloader Reference Documentation](https://jakobharder.github.io/anno-mod-loader)
>
> You can find beginner friendly tutorials in the community [modding guide](https://github.com/anno-mods/modding-guide#readme).

## For Developers

You need Bazel, Visual Studio 2022 and that _should_ be it.
You can checkout [`mail.yml` pipeline](./.github/workflows/main.yml) and see how it's done there.

easy steps to sucess:

- Install Visual Studio 2022 (community version is fine + C++ tools)
- Install Bazel (I recommend going with Bazelisk here)
- _optional_ fork this repo
- clone this repo and its submodules:
  `git submodule update --init --recursive`
- open the folder in Visual Studio 2022
- _optional_ make changes
- open a command prompt (admin) & navigate to the workingdir
- If you have installed another version of Visual Studio as well:
  - `set BAZEL_VC=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC`
  - this will use the correct build tools (if your VS 2022 install dir differ, please adapt)
- use `bazel build ///...` to build the library and test program
- use `bazel test //tests/xml:xml-tests` to run the unit tests
- find the test program in your workingdir \bazel-bin\cmd\xmltest

If you want to work on new features for XML operations, you can use xmltest for testing. As that is using the same code as the actualy file loader.
