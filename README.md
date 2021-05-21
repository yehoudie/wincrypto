# Crypto utils #
Implementations of some useful crypto utils using the windows API.

## REQUIREMENTS ##
- msbuild
- [WDK]

**Remarks**
The .vcxproj files are using `WindowsApplicationForDrivers10.0` as the `PlatformToolset`, which leads to smaller builds. 
If the WDK is not installed, the `PlatformToolset` may be changed to `v142` and it should compile without errors.

## BUILD WHOLE SOLUTION ###
```bash
$ ./winBuild.bat [/t crypto] [/m Release|Debug] [/b 32|64] [/rt] [/pdb] [/bt a\path] [/h]
```
The correct path to your build tools may be passed as a parameter or changed in the script [winBuild.bat](winBuild.bat) itself.  



## SHA256 ##
Calculates sha256 sum of files or files in folders.

### Version ###
1.0.1  
Last changed: 18.05.2021


### BUILD ###
```bash
$ ./winBuild.bat /t sha256 [/m Release|Debug] [/b 32|64] [/rt] [/pdb] [/bt a\path] [/h]
```
The correct path to your build tools may be passed as a parameter or changed in the script [winBuild.bat](winBuild.bat) itself.  

 
### USAGE ###
```bash
$ sha256 [/h] [/c] path...
```

Options:  
 * /c Compare path1 with path2 or path1 with shaValue.
 * /h Print help.
 
 **Examples**
```bash
$ sha256 a/file a/nother/file
$ sha256 a/dir/
$ sha256 /c file1 file2
$ sha256 /c file1 sha256
```


## COPYRIGHT, CREDITS & CONTACT ## 
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).

#### Author ####
- Henning Braun ([henningbraun@gmx.net](henningbraun@gmx.net)) 
