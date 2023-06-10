# Mp1_Practice_Mod
A mod focused on helping with minigame island speedruns

Features:

Display Features:</br>
Displays current main seed rng and amount of rng calls that have occurred</br>
Displays most inputs (besides L and R) pressed</br>
Displays turn count in shy guy says</br>
Displays frame count in bumper balls</br>
Displays scroll speed of camera in pipe maze as percentage</br>
Displays lag frames that have occurred at the bottom right</br>
Displays fps at bottom right</br>
Displays frame count in cyan at bottom right</br>
</br>
Controls:</br>
While on minigame island overworld:</br>
C-up advances the seed forwards by 1</br>
C-down advances the seed backwards by 1</br>
C-right advances the seed forwards by 16</br>
C-left advances the seed backwards by 16</br>
R toggles frame advance while L steps 1 frame forward</br>
Dpad up toggles the box display at the top left of the screen</br>
Dpad left saves the current seed</br>
Dpad down loads the saved seed</br>
Dpad right resets the lag counter</br>
</br>
While in pipe maze minigame:</br>
C-stick up/down will increase/decrease the scroll speed of the camera by 10% (default scroll speed is 150)</br>
</br>
</br>
# Building
The building process currently uses a simple batch file</br>
Follow this tutorial by shygoo for setting up armips https://hack64.net/Thread-Importing-C-Code-Into-SM64-Using-n64chain-and-armips</br>
Once armips is set up, clone this repo and create a new folder in the root of the repo named `rom`</br>
Place a vanilla, mp1 rom named `mp2.z64` in the `rom` folder</br>
Run `build.bat` and it will generate your new modded rom in the `rom` folder named `mp2.mod.z64`
