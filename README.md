# svg_extract
Open source svg to tsp file converter

## What the heck is this?
This is an open source version of the svg_extract available in the [TSP Art webpage](https://drububu.com/illustration/tsp/). The one in this webpage is only available via a windows executable, whereas the one here is open source and is supposed to solve a bug in the former, where it crashed when generating big TSP files (more than 200K cities).

## How is this useful?
The TSP Art is a very interesting idea where you can convert an image to a set of points and later try to solve this set of points as if it was the traveling salesman problem. This workflow, taken from the previously mentioned webpage, summarizes the idea:

![workflow](https://drububu.com/illustration/tsp/images/tsp-workflow.png)

Some crazy random dude could try to generate a huge tsp file (this guy is me), so this can make the svg_extract in the webpage to fail, so this was tested to work with pretty huge images.

## Image example
![example](messi.jpg)
