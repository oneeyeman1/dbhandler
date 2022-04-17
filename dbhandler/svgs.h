#pragma once

const char query[] = R"foo(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:xlink="http://www.w3.org/1999/xlink"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   version="1.1"
   id="svg112"
   width="15"
   height="16"
   viewBox="0 0 15 16"
   sodipodi:docname="query-resized.svg"
   inkscape:version="1.0.2 (e86c870879, 2021-01-15)">
  <metadata
     id="metadata118">
    <rdf:RDF>
      <cc:Work
         rdf:about="">
        <dc:format>image/svg+xml</dc:format>
        <dc:type
           rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
        <dc:title></dc:title>
      </cc:Work>
    </rdf:RDF>
  </metadata>
  <defs
     id="defs116" />
  <sodipodi:namedview
     pagecolor="#ffffff"
     bordercolor="#666666"
     borderopacity="1"
     objecttolerance="10"
     gridtolerance="10"
     guidetolerance="10"
     inkscape:pageopacity="0"
     inkscape:pageshadow="2"
     inkscape:window-width="1403"
     inkscape:window-height="991"
     id="namedview114"
     showgrid="false"
     inkscape:zoom="49.625"
     inkscape:cx="3.5806045"
     inkscape:cy="8"
     inkscape:window-x="26"
     inkscape:window-y="23"
     inkscape:window-maximized="0"
     inkscape:current-layer="g120" />
  <g
     inkscape:groupmode="layer"
     inkscape:label="Image"
     id="g120">
    <image
       width="15"
       height="16"
       preserveAspectRatio="none"
       xlink:href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAQCAYAAADJViUEAAAA70lEQVQoU51TvQ5GQBCc89NQSLQ6
Eq3+Co/i3TyG1mNoRKGWSKiEz16+24hTOFtxuZ2Z3ZkTx1n4WEI3CyEYIssyFEWBZVnQNA2fb9sG
13X5X+z7fjiOgzcC1nVFGIZ8l+islJNCTaSaSccb5nmeEUWRyXydmcCklBjHEcMw8IxEYDC/YdUI
n2X3fY80TU3ZZMG5eZaYJImSfS2yyvM8+23neY6u68yZNXpVVajr2sgb+RsEgTovyxJt26pvw+fr
1u/bVQ1PPtO8d7voMkWUUvW4bR2vOI4xTZMhmdjv8eWEWWXzD62l86uyfZW+7+MHMpKl4FNs51gA
AAAASUVORK5CYII=
"
       id="image122" />
  </g>
</svg>
)foo";

const char odbc[] = R"foo(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:cc="http://creativecommons.org/ns#"
    xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
    xmlns:svg="http://www.w3.org/2000/svg"
    xmlns="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    version="1.1"
    id="svg130"
    width="16"
    height="17"
    viewBox="0 0 16 17">
    <metadata
    id="metadata136">
    <rdf:RDF>
    <cc:Work
    rdf:about="">
    <dc:format>image/svg+xml</dc:format>
    <dc:type
    rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
    <dc:title></dc:title>
    </cc:Work>
    </rdf:RDF>
    </metadata>
    <defs
    id="defs134" />
    <g
    id="g138">
    <image
    width="16"
    height="17"
    preserveAspectRatio="none"
    xlink:href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAARCAYAAADUryzEAAAAAXNSR0IArs4c6QAAAARnQU1BAACx
    jwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAJUSURBVDhPpZPfS1NxGMY/25rLhdZ0kiY0Sl2L
    NmvgLlQwW95ESje5LvsBqxshkP0FQXQxIhTsti7MYolCEgQSWZvdROZAZ6a4sTWnW7YxK9Azt862
    U3EmXfm5eQ+H532e93zf71HkRNgDuw22twnMB/B5N1FoWtkSfrC1lcFi+UhdXZ1YmyVhEaVUC6ST
    SbonXjGS8DKpuU9SdQfnjQzl5Y+YmfnE2lqcgYFB1tfXpY4Sg9dT0zRf6Cb2a5msropngXd8iwnE
    4z9xOm9SW1tLKBQUzWakjhID3/unHB1WoVxNUZN6wW3LW2719SEIGRKJBF6fF5VKhVarlTpKDC71
    9KI/COq5KPURJ9oDSnayC2JyPZ/n5zDp9cwvLPzfoLrahn/OxhWHD0GhIhw5T81hE5WVhzhrtzM2
    9YaXExPYbDapo8Tg4aCJY8c/4L5XT2fnXdFwEvOpcXK5/eh0Oq5eu47b7ZbURXatsb/fjdlyhHQq
    h178nuEn05wwbjIwOITH48HhcEjKIjKDxcVFotFVvkYiCBqBSCiMyXCSYChEY2Mjvb2XJeU/ZAYr
    K0HGx8fIClkCuS+MPHjMxbYelPuUjI6OSio5e77KskPMs7S0VKixWIykeDPzLC8X36XTacLhcOH5
    D7IJ/H4/GxvfUShgdnaWhoYGcRuduFwuurq6KCsrw2AwYDabUavVhR7ZBPkEu/0cQmYHo9FIU1NT
    YYp8akVFBVVV1VitVkldRGbQ2t6Ob2iItjOnSaZS4s+zVkjs6OigpaVFnEAtrvL53/Q8ezxE+A0a
    Y/PFsit2QwAAAABJRU5ErkJggg==
    "
    id="image140" />
    </g>
    </svg>
    )foo";
