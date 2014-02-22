QtGuilt
=======
GUI over guilt (quilt on top of git)

Warning : This is alpha software. 
-------
 
This project implements a gui over the guilt utility and adds some additionnal functionnalities. 
It also (will) add the ability to use it over a local subversion repository. 
It is pure Qt4 implementation. 

The visualisation works well, but everything has not been fully tested for now and it may ruin your repository.


### Build (build out of sources):
    mkdir GtGuilt-build
    cd GtGuilt-build
    qmake PATH/TO/SOURCES/QtGuilt.pro
    make
### For installing in custom location, this is the same procedure except the qmake step : 
    qmake PATH/TO/SOURCES/QtGuilt.pro PREFIX=customPath
### Install (as root or with sudo if not using PREFIX): 
    make install

### Features (not all listed for now)
* legend (The features are preceded by letters) : 
 * M implemented in model
 * G implemented in GUI
 * W warning : may break things
 * M without G means that it is not available in the GUI but implemented in model
* [MG] View the patch stack in one look
 * The view permits manipulations and manipulations (guilt commands directly in the repo) updates the view 
* [MG] Shows if top patch is not refreshed
* "Pure guilt" Features :
 * [MG] push
 * [MG] pop
 * [MG] "goto" : double click on a patch an it becomes top
 * [MG] rename
 * [MGW] edit/view the patch directly
 * [MG] edit/view the patch header
 * ...
* Additional Features : 
 * [MV] View the patches in e tree manner (PatchName, Mofified files, Modifications chunks)
 * [MGW] Apply unrefreshed modifications on existing or new patch
 * [MG] Desactivate/Activate a patch
 * [MG] Change patches order
 * [M] Extract a modified file from a patch
 * [M] Move a modified file from a patch to an other
 * [MGW] Split a patch into a list of patches, one for each modified files
 * ...

