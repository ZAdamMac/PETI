# Welcome to the PETI Contributor Guidelines!
Hey, thanks for checking us out! PETI's renaissance phase of development is ambitious at best, which means we're always looking for contributors to come in and help us make a best effort at producing something truly useful for teaching introductory concepts in hardware hacking.

First off, let me just say that any contribution matters - from pull-requesting in new functions, to requesting new features, or even just reporting your crashes and bugs. Right now a lot of development is underway, but it's at a hobby pace.
 
PETI doesn't have automated crash and bug reporting, and by its very nature, never can. I'm just one guy with a few spare hours in the evening trying to put this thing together. Everything you contribute helps.

Secondly, we do have a few resources for the eager:
- [**Discord Server:**](https://discord.gg/34KJfkg) easily the fastest way to join in the conversation or ask a question;
- [Email Development Team](mailto:petidev@arcanalabs.ca);
- and, of course, this guide!

# Development Team
While everyone who contributes to the project is part of our development team, the capital-letters Development Team currently consists of github user ZAdamMac.

You don't need to join the Development Team to submit issues or PRs, but if you want to get your hands dirty as a more deeply-involved voice in the future of this project, drop me a line.

The Development Team has the ultimate responsibility for approving and merging PRs, enhancing and maintaining the build tooling, and generally guiding the overall development of the project.

# Feature Requests
We're always happy to have a look at requests for new features. They further our quest for maximum utility and usability.

## Filing a Feature Request
If you have a feature request for PETI, go ahead and open a new issue at this repo. Be sure to head the title of the request with `[RFC]`. The development team will cross-post your request in announcements to the discord and any other contact trains we happen to be using at the time, opening the request up for commentary by other members. **Roughly 30 days later**, the same group will decide whether or not to include the feature, or some version of it, into the development process. If the feature is complicated or discussion over it is lively, this decision process may be extended as necessary to allow the matter to resolve. Likewise, if there isn't much discussion to be had, we might just jump on it right away, if deemed necessary.

# Bug Reports
We kind of can't overstate the importance of providing your bug reports to us. It's so important we've made a template for them. When PETI crashes on your system, we have no idea what happened or why. Please be encouraged to file an issue and fill out the bugs form. Super important stuff.

# Contributing with Code: The Pull Request!
For PETI, we use Fork and Request. If you want to contribute to the development process with your code, the preferred method is to first fork the repo, make your changes there, and then submit a pull request. For obvious reasons all submissions are subject to review. To make this whole process easier, be sure to use nice, clear, descriptive code. Provide documentation and comments as needed.

In general though, we're super happy to have any help that we can. People who contribute with code are being kept track of and will be included in the credits.

## Branching
For the sake of convenience we have three different kinds of branch, with a two-character prefix on the branch name to denote them:
- `h/` (mnemonic: housekeeping) branches for adding and maintaining documentation, licensing, or other paperwork tasks that don't affect the main source files located in /src. This can include maintaining or upgrading the build pipeline tooling, changing doxygen configs, and so on. Merging `h/` branches does not change the firmware version numbering for obvious reasons.
- `f/` (mnemonic: feature) branches for adding functionality to the PETI firmware itself. An `f/` branch should introduce a new scene, feature, font, etc but may also include bug-fixes depending on timing constraints. 
- `b/` (mnemonic: bugfix) branches for quickly correcting issues. If branched from a different branch and merged back to that branch they have no impact on the version numbering. If branched off of and back into `main`, they increment the least significant digit of the firmware version number, e.g. `0.1.0 -> 0.1.1`.
- `v/` (mnemonic: version). Versions are based from `main` at a certain moment in time and set aside for feature work. Feature branches should branch off of the current `v/` branch. This usually changes the `middle digit` of the firmware version number, for example an `f/` branched off of `0.1.0` would be merged to create `0.2.0`. (Prior to 2022/10/12 this was not the expected behaviour.)

If you're unclear on this convention please ask in the `#peti` channel on the lab discord. In general, you should almost always fork from and PR against `main`.

## Comment Convention
`TODO and FIXME` are comment prefixes indicating immediate action is required. FIXME comments should never be commited to any branch of the main repo but are naturally permitted on your private fork as suits your development practice. TODO commits are permitted on all branches *except* on `v/` version compliation branches.

A special convention exists for TODO comments that for one reason or another are not resolvable in the next feature release: `FUTURE`. These are to be used for features that would be "too heavy" to implement immediately or are hard gated behind other work not yet in scope. 

## Testing
A full suite of tests is desired but not currently on the roadmap. Current functional testing is the responsibility of the person handling the merge request and should include a comprehensive test of both new and old functionality for a compiled version of the code being submitted on a PETI development kit.

As of 2022/10/12 until this note is removed, zadammac is the only person with such a devkit and must be involved in all testing efforts. This is likely to be the case until issue #20, a bug in the development kit's hardware design, is resolved.

# Contributing with Coins
PETI is and ever will be free-as-in-beer firmware (and, at least for the schematics, hardware), provided at no charge and by donation to users of all scales.

As a freeware project, we're obviously working with the bare minimum funding possible - luckily, this sort of thing also doesn't take much. If you'd like to donate to keep PETI's development alive, a one-time donation can be made by purchasing us some ~~debugger fluid~~ coffee via Ko-Fi.com:

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A8614WZ3)

As a note, this is my personal Ko-Fi - if this ever becomes the work of a full-blown development group an organizational account will be created.

If you want to support PETI's development on an ongoing basis, [consider backing ZAdamMac through Github Sponsors](https://github.com/users/ZAdamMac/sponsorship). This is an especially helpful case as Github is currently matching sponsors' donations in order to promote the Sponsors platform.
