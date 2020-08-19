## Maintaining the GitHub web site

This website will just contain the web files and any files needed by them. The repository from which these files are generated is your fork of the [VTK Examples Repository](__SITE_REPOSITORY__). For this discussion we are assuming that you have cloned your fork of this site into a repository called `__REPO_NAME__`.

## Setup for Updating the Web Pages

This assumes you have `VTK`, `CMake`, `git`, `git lfs`, `mkdocs`, `pygments` and `htmlmin` installed along with the checkout of your fork of the [VTK Examples Repository](__SITE_REPOSITORY__).

### Fork the repository

Go to the [VTK Examples Web Site Repository](__WEB_REPOSITORY__) sign in/register, and then fork the repository.

A fork is a copy of a project. Forking a repository allows you to make changes without affecting the original project.

If you want to see what your build of the web pages will look like, go to your settings. Then scroll down until you find **GitHub Pages** and set the Source Branch to **gh-pages**.

### Clone the VTK Examples Web Site Repository

``` bash
git clone git@github.com:<github_username>/vtk-examples.git vtk-examples-web
cd vtk-examples-web
```

### Add the VTK Examples Web Site Repository as a *remote* called *upstream*

``` bash
cd vtk-examples-web
git remote add upstream __WEB_REPOSITORY__
```

## Updating web pages

For this discussion we are assuming a directory structure something like this:

``` bash
|--
  |-- VTK               # The VTK Source files
  |-- vtk-examples      # The VTK Examples source files
  |-- vtk-examples-web  # The VTK Examples Web Source files

```

### Step 1: Sync your **vtk-examples** repository

Remember that to run the following commands, you need to be in your `__REPO_NAME__` folder.

``` bash
cd __REPO_NAME__
git fetch upstream
git switch master
git merge upstream/master
git push
```

**Note:** You can use your own unmerged branch here but do not submit a merge request as it will be out of sync with the __REPO_NAME__ master.

### Step 2: Sync your **vtk-examples-web** repository and checkout **gh-pages**

Sync your repository with the [VTK Examples Web Site Repository](__WEB_REPOSITORY__). Remember that to run the following commands, you need to be in your **vtk-examples-web** folder.

``` bash
cd vtk-examples-web
git fetch upstream
git switch master
git merge upstream/master
git push
git switch gh-pages
```

**Note:** If you are doing this for the first time replace: `git switch gh-pages` with `git switch -c gh-pages  --track origin/gh-pages`

**Important:** You must be on the **gh-pages** branch in **vtk-examples-web** before doing the next steps. So remember to do `git switch gh_pages`!

### Step 3: Build the web site

Go to the `__REPO_NAME__` folder, ensure that the file `mkdocs.yml` has the correct parameters.

Then in the top-level folder of `__REPO_NAME__`:

``` bash
../src/SyncSiteWithRepo.sh SITE_URL WEB_SITE_URL WEB_REPO_URL WEB_DIR VTK_SOURCE_DIR
```

where:

- `SITE_URL` is the VTK Examples source repository URL, e.g. __SITE_REPOSITORY__
- `WEB_SITE_URL` is the VTK Examples website site URL, e.g. __WEB_SITE_URL__
- `WEB_REPO_URL` is the VTK Examples website source URL, e.g. __WEB_REPOSITORY__
- `WEB_REPO_DIR` is the local website source dir, e.g. **local_path**/vtk-examples-web
- `VTK_DIR` is the local VTK source directory, e.g. **local_path**/VTK

Remember that local paths are absolute or full paths, not relative paths.

Then, once this script finishes:

### Step 4: Verify that the site works

Go to the folder mentioned in the instructions at the end of the script and verify that the site works.

### Step 5: Commit, Add and Push

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

### Step 6: Check it all works

Verify that the links etc. are OK on `https://<username>.github.io/vtk-examples/site/`

**Do not** submit a merge request. This needs to be done by an admisistrator. 

### Step 7: For Administrators

If you are an administrator and:

- You had checked out the master in step 1.
- The web site `https://<username>.github.io/vtk-examples/site/` is OK with all the links etc. working.

 Then go to your fork `https://github.com/<username>/vtk-examples` and submit a merge request. 
