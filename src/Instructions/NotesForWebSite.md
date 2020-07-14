# Notes for creating the GitHub web site

This website will just contain the web files and any files needed by them. The repository from which these files are generated is [VTK Examples](https://gitlab.kitware.com/vtk/vtk-examples). You need to have cloned this site by default into a repository called `vtk-examples`.

## Creating the site

Create the site on GitHub, do not use the same name as the Kitware repository namely `vtk-examples`.
Choose a name like `vtk-examples-web`. This means that when you clone the repository, you don't have to select a different name as the repository `vtk-examples` already exists.

## Cloning

Clone the site and you should have a folder `vtk-examples-web`. For convenience this should be in the same folder as your local [vtk-examples](https://gitlab.kitware.com/vtk/vtk-examples) repository.

## After checkout

Go to the `vtk-examples`, ensure that the file `mkdocs.yml`has the correct parameters.

Then in the top-level folder of `vtk-examples`:

``` bash
./SyncSiteWithRepo REPO_URL REPO_DIR VTK_SOURCE_DIR
```

where:

- `REPO_URL`: The URL of the repository where the web pages are.
- `REPO_DIR`: The local directory of the web pages repository.
- `REPO_URL`: The URL of the `vtk-examples` repository.

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

In the top-level folder of `vtk-examples`:

``` bash
./SyncSiteWithRepo REPO_URL REPO_DIR VTK_SOURCE_DIR
```

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
