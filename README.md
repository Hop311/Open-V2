# Open-V2 - Installation guide

#### 1 Visual Studio
Install [Visual Studio 2022](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022).
#### 2 Boost
Download [boost](https://www.boost.org/users/history/version_1_81_0.html) and put `boost_1_81_0\boost` into `lib` (so that you have `lib\boost\<headers>`).
#### 3 Vic2 Install
If your Vic2 install location is different from
```
C:\Program Files (x86)\Steam\steamapps\common\Victoria 2
```
you must update `VIC2_INSTALL_DIR` in `file_explorer\file_explorer.cpp` (line 218) with the correct path.
#### 4 Build
Run `build.bat`, specifying `Debug` or `Release`, or leaving it blank for the default (`Debug`). Once the script finishes, run `..\open_v2_test_data\[Debug|Release]\file_explorer.exe`, which will generate `test_save_cmp.bin` and `test_scenario.bin` before closing. Run it again to start `Open V2`.
