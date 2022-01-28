#!/bin/bash
#
# SyncSiteWithRepo.sh - synchronize the examples site with the
#                    examples repository
#
# These Python packages are needed: mkdocs, pygments, htmlmin
# Run: pip install mkdocs-material
# If needed.
#
# A typical command would look something like this:
# ./src/SyncSiteWithRepo.sh https://gitlab.kitware.com/vtk/vtk-examples https://kitware.github.io/vtk-examples/site/ https://github.com/Kitware/vtk-examples <local-path>/vtk-examples-web/ <local-path>/VTK/
#
if [ $# -lt 5 ]
  then
  echo "Usage: SyncSiteWithRepo SITE_URL WEB_SITE_URL WEB_REPO_URL WEB_DIR VTK_SOURCE_DIR"
  echo "Where:"
  echo "  SITE_URL is the VTK Examples source repository URL, e.g. https://gitlab.kitware.com/vtk/vtk-examples"
  echo "  WEB_SITE_URL is the VTK Examples website site URL, e.g. https://<username>.github.io/<site_name>/site"
  echo "  WEB_REPO_URL is the VTK Examples website source URL, e.g. https://github.com/<username>/<site_name>"
  echo "  WEB_REPO_DIR is the local website source dir, e.g. <local_path>/<site_name>-web"
  echo "  VTK_SOURCE_DIR is the local VTK source directory, e.g. <local_path>/src/VTK"
  echo "Notes:"
  echo " 1. Paths must be specified as absolute or full paths, not relative paths."
  echo " 2. This is run from the top-level VTKExamples directory. e.g <local_path>/vtk-examples/"
  exit 1
fi
SITE_URL=$1
WEB_SITE_URL=$2
WEB_REPO_URL=$3
WEB_REPO_DIR=$4
VTK_SOURCE_DIR=$5

start_time=$(date +%s)

echo "Synchronizing the vtk-examples site with the web repository folder."
echo " VTK Examples site URL: "$SITE_URL
echo " Web Site URL:          "$WEB_SITE_URL
echo " Web Repository URL:    "$WEB_REPO_URL
echo " Web Repository DIR:    "$WEB_REPO_DIR
echo " VTK Source DIR:        "$VTK_SOURCE_DIR


echo "1) Make the directories (if needed)"
mkdir -p ${WEB_REPO_DIR}/docs
mkdir -p ${WEB_REPO_DIR}/src
mkdir -p ${WEB_REPO_DIR}/site
mkdir -p ${WEB_REPO_DIR}/_layouts
mkdir -p ${WEB_REPO_DIR}/custom_theme
mkdir -p ${WEB_REPO_DIR}/src/Artifacts
mkdir -p ${WEB_REPO_DIR}/src/Images
mkdir -p ${WEB_REPO_DIR}/src/stylesheets
mkdir -p ${WEB_REPO_DIR}/src/SupplementaryData
mkdir -p ${WEB_REPO_DIR}/src/Testing/Baseline
mkdir -p ${WEB_REPO_DIR}/src/VTKBook/Figures
mkdir -p ${WEB_REPO_DIR}/src/VTKFileFormats/Figures

echo "2) Copy the baseline images and other essential files"
rsync -zavh index.html ${WEB_REPO_DIR}
rsync -zavh LICENSE ${WEB_REPO_DIR}
rsync -zavh mkdocs.yml ${WEB_REPO_DIR}
rsync -zavh VTKColorSeriesPatches.html ${WEB_REPO_DIR}
rsync -zavh VTKNamedColorPatches.html ${WEB_REPO_DIR}
rsync -zavh _layouts/ ${WEB_REPO_DIR}/_layouts
rsync -zavh custom_theme/ ${WEB_REPO_DIR}/custom_theme
rsync -zavh src/Artifacts/ ${WEB_REPO_DIR}/src/Artifacts
rsync -zavh src/Images/ ${WEB_REPO_DIR}/src/Images
rsync -zavh src/stylesheets/ ${WEB_REPO_DIR}/src/stylesheets
rsync -zavh src/SupplementaryData/ ${WEB_REPO_DIR}/src/SupplementaryData
rsync -zavh src/Testing/Baseline/ ${WEB_REPO_DIR}/src/Testing/Baseline
rsync -zavh src/VTKBook/Figures/ ${WEB_REPO_DIR}/src/VTKBook/Figures
rsync -zavh src/VTKFileFormats/Figures/ ${WEB_REPO_DIR}/src/VTKFileFormats/Figures
cp web_gitignore ${WEB_REPO_DIR}/.gitignore

echo "3) Create coverage files"
(cd src/Admin; python ./VTKClassesUsedInExamples.py -a .. ${WEB_REPO_DIR}/src/Coverage)

echo "4) Scrape the repo building the markdown files"
rm -rf ${WEB_REPO_DIR}/docs/*
rm -rf ${WEB_REPO_DIR}/site/*

src/Admin/ScrapeRepo.py src ${SITE_URL} ${WEB_SITE_URL} ${WEB_REPO_URL} ${WEB_REPO_DIR} ${VTK_SOURCE_DIR}

echo "5) Check for a successful scrape"
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

echo "6) Build the html pages"
mkdir -p ${WEB_REPO_DIR}/docs/stylesheets
cp ${WEB_REPO_DIR}/src/stylesheets/extra.css ${WEB_REPO_DIR}/docs/stylesheets/extra.css
pushd ${WEB_REPO_DIR}
mkdocs build
popd

######################
# echo "Premature exit for testing"
# exit

#####################
echo "7) Minify Html"
pushd ${WEB_REPO_DIR}/site
find . -name index.html -exec htmlmin {} {} \;
popd

#####################
end_time=$(date +%s)
total_time=$(expr $end_time - $start_time)
echo
echo "Total time taken: "$total_time"s"
echo
#####################
echo "Now go to the repository DIR: "$WEB_REPO_DIR
echo " and update the site."
