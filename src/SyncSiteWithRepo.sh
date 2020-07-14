#!/bin/bash
#
# SyncSiteWithRepo.sh - synchronize the examples site with the
#                    examples repository
#
# These Python packages are needed: mkdocs, pygments, htmlmin
#
if [ $# -lt 5 ]
  then
  echo "Usage: SyncSiteWithRepo SITE_URL WEB_SITE_URL WEB_REPO_URL WEB_DIR VTK_SOURCE_DIR"
  echo "Where:"
  echo "  SITE_URL is the VTK Examples source repository URL, e.g. https://gitlab.kitware.com/vtk/vtk-examples"
  echo "  WEB_SITE_URL is the VTK Examples website site URL, e.g. https://<username>.github.io/<site_name>/site"
  echo "  WEB_REPO_URL is the VTK Examples website source URL, e.g. https://github.com/<username>/<site_name>"
  echo "  WEB_REPO_DIR is the local website source dir, e.g. <local_path>/<site_name>"
  echo "  VTK_DIR is the local VTK source directory, e.g. <local_path>/src/VTK"
  echo "  e.g  : ./src/SyncSiteWithRepo.sh https://gitlab.kitware.com/vtk/vtk-examples https://ajpmaclean.github.io/web-test/site https://github.com/ajpmaclean/web-test ../web-test ../VTK"
  echo "Note : Paths must be specified as full paths."
  echo "Note : This is run from the top-level VTKExamples directory."
  echo "       e.g <local_path>/vtk-examples/"
  exit 1
fi
SITE_URL=$1
WEB_SITE_URL=$2
WEB_REPO_URL=$3
WEB_REPO_DIR=$4
VTK_DIR=$5

echo "Synchronizing the vtk-examples site with the web repository folder."
echo " VTK Examples site URL: "$SITE_URL
echo " Web Site URL:          "$WEB_SITE_URL
echo " Web Repository URL:    "$WEB_REPO_URL
echo " Web Repository DIR:    "$WEB_REPO_DIR
echo " VTK Source DIR:        "$VTK_DIR

echo "1) Copy the baseline images and other essential files"
rsync -rv --append-verify index.html ${WEB_REPO_DIR}
rsync -rv --append-verify LICENSE ${WEB_REPO_DIR}
rsync -rv --append-verify mkdocs.yml ${WEB_REPO_DIR}
rsync -rv --append-verify VTKColorSeriesPatches.html ${WEB_REPO_DIR}
rsync -rv --append-verify VTKNamedColorPatches.html ${WEB_REPO_DIR}
rsync -rv --append-verify _layouts/ ${WEB_REPO_DIR}/_layouts
rsync -rv --append-verify custom_theme/ ${WEB_REPO_DIR}/custom_theme
rsync -rv --append-verify src/Artifacts/ ${WEB_REPO_DIR}/src/Artifacts
rsync -rv --append-verify src/Images/ ${WEB_REPO_DIR}/src/Images
rsync -rv --append-verify src/stylesheets/ ${WEB_REPO_DIR}/src/stylesheets
mkdir -p ${WEB_REPO_DIR}/src/Testing/Baseline/
rsync -rv --append-verify src/Testing/Baseline/ ${WEB_REPO_DIR}/src/Testing/Baseline/
cp web_gitignore ${WEB_REPO_DIR}/.gitignore

echo "2) Create coverage files"
(cd src/Admin; python ./VTKClassesUsedInExamples.py -a .. ${WEB_REPO_DIR}/src/Coverage)
(cd src/Admin; python ./VTKClassesUsedInExamples.py -a -u  .. ${WEB_REPO_DIR}/src/Coverage)

echo "3) Scrape the repo"
rm -rf build/docs/*
rm -rf build/site/*
src/Admin/ScrapeRepo.py src ${SITE_URL} ${WEB_SITE_URL} ${WEB_REPO_URL} ${WEB_REPO_DIR} ${VTK_DIR}

echo "4) Check for a successful scrape"
pushd ${WEB_REPO_DIR}/docs
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
mkdir -p ${WEB_REPO_DIR}/docs/stylesheets
cp ${WEB_REPO_DIR}/src/stylesheets/extra.css ${WEB_REPO_DIR}/docs/stylesheets/extra.css
pushd ${WEB_REPO_DIR}
mkdocs build
popd

echo "6 Modify highlight color to semitransparent Lavender"
pushd ${WEB_REPO_DIR}/site/assets/stylesheets
sed -i -e 's/background-color:rgba(255,235,59,\.5)/background-color:rgba(230,230,250,0.6)/g' application.*.css
popd

######################
#echo "Premature exit for testing"
#exit

#####################
echo "7) Minify Html"
pushd ${WEB_REPO_DIR}/site
find . -name index.html -exec htmlmin {} {} \;
popd

echo "Now go to the repository DIR: "$WEB_REPO_DIR
echo " and update the site."


#pushd ${WEB_REPO_DIR}
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
