#!/bin/bash

# Verify a version number is specified
if [ $# != 1 ]; then
    echo "Command should be run with a version number specified as its argument."
    exit 1;
fi

# Read in the version number from the command line
VERSION=$1
# Update the version string in the main.cpp file
sed -i "s/version\[\] = \".*\"/version\[\] = \"${VERSION}\"/" src/main.cpp
# Commit modified main.cpp file to the repository
git add src/main.cpp
git commit -m "Updating the version number."
# Set the version tag in the repo
git tag -f v${VERSION}
# Prepare the tarball
git archive --format=tar --prefix=rekollect-${VERSION}/ v${VERSION} | gzip > rekollect-${VERSION}.tar.gz
