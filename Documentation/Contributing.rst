Contributing 
==============
TODO_DOC: move this to a more general page, not Katydid

## Git
Katydid uses the Git version control system.  There are many resources available on the Web if you're new to Git or just need to brush up on the commands:

* GitHub has a [Help](http://help.github.com/) page.
* The [git community book](http://book.git-scm.com/) is a very thorough resource.
* For a reminder about various commands, check out a [cheat sheet](http://help.github.com/git-cheat-sheets/).

## Branching model
Git development follows a particular branching model that has proven effective elsewhere.  The model is described in detail, with helpful diagrams [here](http://nvie.com/posts/a-successful-git-branching-model/).  I'll summarize the main points:

The master branch should always contain stable, ready-to-use code.  Periodically tagged releases will be made from the master branch.

The primary location for code development is in (or around) the develop branch.  These are changes meant for the next release.  Both the master and develop branches will live on the remote repository.  All of the other branches can exist only on local repositories, or they can be pushed to GitHub.

Development work can either be done using the develop branch, or using temporary "feature" branches created from, and eventually merged back into, the develop branch.  Development projects that are large, or will take a long time, should always be done in temporary feature branches.  When the work in a feature branch is ready for use, it should be merged into the develop branch.

In preparation for a release, once all of the features bound for the release are ready, a release branch will be created from the develop branch.  From that point until the release, only bugfixes are allowed in the release branch, and further changes to the develop branch will be aimed for the following release.  After testing is complete the release branch is merged into the master branch, and a tagged version is created.

If bugs are discovered in the master branch, they are fixed in a hotfix branch (or directly in the master branch), and a new tag is created with a revision number.