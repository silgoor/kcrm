#!/bin/bash
cd ~/Documents/Qt/build-kcrm-Desktop_Qt_5_13_2_clang_64bit-Release
~/Qt5.13.2/5.13.2/clang_64/bin/macdeployqt kcrm.app  -codesign="Developer ID Application: Vladimir  Kaufman (HJ8FTL2WWT)"

echo "the end!"
exit 0
