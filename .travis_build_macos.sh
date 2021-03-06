#!/bin/bash

echo "*****************************"
echo "Building SocNetV for macOS..."
echo "*****************************"

APP_NAME="SocNetV"


# Check current directory
project_dir=$(pwd)
echo "Project dir is: ${project_dir}"


echo "TRAVIS_TAG is: $TRAVIS_TAG"
echo "TRAVIS_COMMIT is: $TRAVIS_COMMIT"
echo "SOCNETV_VERSION is: $SOCNETV_VERSION"

LAST_COMMIT_SHORT=$(git rev-parse --short HEAD)
echo "LAST_COMMIT_SHORT is: $LAST_COMMIT_SHORT"

# linuxdeployqt always uses the output of 'git rev-parse --short HEAD' as the version.
# We can change this by exporting $VERSION environment variable 

if [ ! -z "$TRAVIS_TAG" ] ; then
    # If this is a tag, then version will be the tag, i.e. 2.5 or 2.5-beta
    VERSION=${TRAVIS_TAG}
else 
    # If this is not a tag, the we want version to be like "2.5-beta-a0be9cd"
    VERSION=${SOCNETV_VERSION}-${LAST_COMMIT_SHORT}
fi



# Output macOS version
echo "macOS version is:"
sw_vers

# Install npm appdmg if you want to create custom dmg files with it
# npm install -g appdmg


# Build your app
echo "*****************************"
echo "Start building NOW..."
echo "*****************************"

cd ${project_dir}
qmake -config release
make -j4

# Build and run your tests here
find .

# Package your app
echo "*****************************"
echo "Packaging ${APP_NAME}..."
echo "*****************************"
cd ${project_dir}/

# Remove build directories that you don't want to deploy
rm -rf moc
rm -rf obj
rm -rf qrc

echo "Creating dmg archive..."
echo "TAG_NAME is ${TAG_NAME}"

macdeployqt ${APP_NAME}.app -dmg
mv ${APP_NAME}.dmg "${APP_NAME}-${VERSION}.dmg"

# You can use the appdmg command line app to create your dmg file if
# you want to use a custom background and icon arrangement. I'm still
# working on this for my apps, myself. If you want to do this, you'll
# remove the -dmg option above.
# appdmg json-path ${APP_NAME}_${TRAVIS_TAG}.dmg

# Copy other project files
cp "${project_dir}/README.md" "README.md"
cp "${project_dir}/COPYING" "LICENSE"

echo "Packaging zip archive..."
7z a ${APP_NAME}-${VERSION}-macos.zip "${APP_NAME}-${VERSION}.dmg" "README.md" "LICENSE"

echo "Check what we have created..."
find . -type f -name "${APP_NAME}*"

echo "Done!"

exit 0
