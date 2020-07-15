## Maintaining the GitHub web site

This website will just contain the web files and any files needed by them. The repository from which these files are generated is [VTK Examples](__SITE_REPOSITORY__). You need to have cloned this site by default into a repository called `__REPO_NAME__`.

## Creating the site

Create the site on GitHub, do not use the same name as the Kitware repository namely `__REPO_NAME__`.
Choose a name like `vtk-examples-web`. This means that when you clone the repository, you don't have to select a different name as the repository `vtk-examples` already exists.

You will need to set the GitHub Pages site to the master branch in the site settings.

## Cloning

Clone the site and you should have a folder  called, for example: `vtk-examples-web`. For convenience this should be in the same folder as your local [__REPO_NAME__](__SITE_REPOSITORY__) repository.

## After checkout

Go to the `__REPO_NAME__` folder, ensure that the file `mkdocs.yml` has the correct parameters.

Then in the top-level folder of `__REPO_NAME__`:

``` bash
../src/SyncSiteWithRepo.sh SITE_URL WEB_SITE_URL WEB_REPO_URL WEB_DIR VTK_SOURCE_DIR
```

where:

- `SITE_URL` is the VTK Examples source repository URL, e.g. __SITE_REPOSITORY__
- `WEB_SITE_URL` is the VTK Examples website site URL, e.g. __WEB_SITE_URL__
- `WEB_REPO_URL` is the VTK Examples website source URL, e.g. __WEB_REPOSITORY__
- `WEB_REPO_DIR` is the local website source dir, e.g. <local_path>/__WEB_REPO_NAME__
- `VTK_DIR` is the local VTK source directory, e.g. <local_path>/VTK

## First commit

Use `git add` to add the following folders:

``` bash
.gitignore
LICENSE
Tarballs/
VTKColorSeriesPatches.html
VTKNamedColorPatches.html
site/
src/
```

Then

``` bash
git commit -m "Updating the site"
git push
```

It will take a while for the site to appear.

## Subsequent commits

In the top-level folder of `__REPO_NAME__`:

``` bash
./SyncSiteWithRepo SITE_URL WEB_SITE_URL WEB_REPO_URL WEB_DIR VTK_SOURCE_DIR
```

For an explanation of the parameters, see [After checkout](#After-checkout)

Then after a while:

``` bash
git status
git commit -a -m"Updating files"
git push
```

If there are new files:

``` bash
git status
git add <The files>
git commit -m"Adding new files"
git push
```
