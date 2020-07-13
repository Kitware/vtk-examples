#!/bin/bash
#
# SyncSiteWithRepoPy3 - synchronize the examples site with the
#                    examples repository
#
# These Python packages are needed: mkdocs, pygments, htmlmin
#
if [ $# -lt 2 ]
  then
  echo "Usage: SyncSiteWithRepo REPO_URL REPO_DIR VTK_SOURCE_DIR"
  echo "e.g  : ./src/SyncSiteWithRepo.sh https://github.com/<github_username>/VTKExamples /home/<username>/Development/Kitware/src/web-test /home/<username>/Development/Kitware/src/VTK/"
  echo "e.g  : ./src/SyncSiteWithRepo.sh https://github.com/ajpmaclean/VTKEx /home/amaclean/Development/Kitware/src/web-test /home/amaclean/Development/Kitware/src/VTK/"
  echo "Note  : This is run from the top-level VTKExamples directory."
  echo "e.g  : /home/<username>/Development/Kitware/src/VTKExamples/"
  exit 1
fi
REPO_URL=$1
REPO_DIR=$2
VTK_DIR=$3

echo "Synchronizing the vtk-examples site with the repository folder."
echo "Repository URL: "$REPO_URL
echo "Repository DIR: "$REPO_DIR
echo "VTK Source DIR: "$VTK_DIR

echo "1) Copy the baseline images and other essential files"
rsync -rv --append-verify index.html ${REPO_DIR}
rsync -rv --append-verify LICENSE ${REPO_DIR}
rsync -rv --append-verify mkdocs.yml ${REPO_DIR}
rsync -rv --append-verify VTKColorSeriesPatches.html ${REPO_DIR}
rsync -rv --append-verify VTKNamedColorPatches.html ${REPO_DIR}
rsync -rv --append-verify _layouts/ ${REPO_DIR}/_layouts
rsync -rv --append-verify custom_theme/ ${REPO_DIR}/custom_theme
rsync -rv --append-verify src/Artifacts/ ${REPO_DIR}/src/Artifacts
rsync -rv --append-verify src/Images/ ${REPO_DIR}/src/Images
rsync -rv --append-verify src/stylesheets/ ${REPO_DIR}/src/stylesheets
mkdir -p ${REPO_DIR}/src/Testing/Baseline/
rsync -rv --append-verify src/Testing/Baseline/ ${REPO_DIR}/src/Testing/Baseline/
cp web_gitignore ${REPO_DIR}/.gitignore

echo "2) Create coverage files"
(cd src/Admin; python ./VTKClassesUsedInExamples.py -a .. ${REPO_DIR}/src/Coverage)
(cd src/Admin; python ./VTKClassesUsedInExamples.py -a -u  .. ${REPO_DIR}/src/Coverage)

echo "3) Scrape the repo"
rm -rf build/docs/*
rm -rf build/site/*
src/Admin/ScrapeRepo.py src ${REPO_DIR} ${REPO_URL} ${VTK_DIR}

echo "4) Check for a successful scrape"
pushd ${REPO_DIR}/docs
count=$((`find . -name \*.md | wc -l`))
popd
expected=800
if test $count -lt $expected; then
   echo Admin/ScrapeRepo failed
   echo Expected at least $expected markdown files but only found $count files
   git checkout .
   exit 1
fi

echo "5) Update the html pages"
mkdir -p ${REPO_DIR}/docs/stylesheets
cp ${REPO_DIR}/src/stylesheets/extra.css ${REPO_DIR}/docs/stylesheets/extra.css
pushd ${REPO_DIR}
mkdocs build
# This file is around 118MB
# GitHub's file size limit of 100.00 MB
#rm site/search/search_index.json
popd

echo "6 Modify highlight color to semitransparent Lavender"
pushd ${REPO_DIR}/site/assets/stylesheets
sed -i -e 's/background-color:rgba(255,235,59,\.5)/background-color:rgba(230,230,250,0.6)/g' application.*.css
popd

######################
#echo "Premature exit for testing"
#exit

#####################
echo "7) Minify Html"
pushd ${REPO_DIR}/site
find . -name index.html -exec htmlmin {} {} \;
popd

echo "Now go to the repository DIR: "$REPO_DIR
echo " and update the site."


#pushd ${REPO_DIR}
#echo "8) Process modified files"
#git commit -m "SYNC: Files modified in the repo." `git status | grep modified: | cut -d":" -f2,2`
#
#echo "8.1) Process new files"
#find . "(" -name \*.html ")" -exec git add {} \;
#(cd site; git add assets)
#git commit -m "SYNC: Files added to the repo."
#
#echo "8.2) Process deleted files"
#git rm `git status | grep deleted: | cut -d":" -f2,2`
#git commit -m "SYNC: Files deleted (or moved) from the repo."
#
#echo "9) Update tarballs and push to tarball repo"
#if ( test -d src/Tarballs ); then
#(cd src/Tarballs; git add *tar)
#(cd src/Tarballs; git commit -m "SYNC: Tarballs modified")
#(cd src/Tarballs; git push)
#fi
#
#echo "10) Push the changes"
#git push
#popd
