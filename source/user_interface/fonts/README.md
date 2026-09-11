# Fonts

This sample demonstrates text display features in UNIGINE across six languages - English, Russian, Arabic, Thai, Chinese, and Hindi - including right-to-left and complex script support. The same text is rendered on GUI widgets, a 3D *ObjectText*, and a world-space *ObjectGui* nodes.

The **Parameters** window allows you to control:

-**Text direction** - Auto-detects direction per string (LTR/RTL force one direction for every widget at once).
-**Cursor mode** - Defines how the caret steps through bidirectional text: *Auto, Visual*, or *Logical*. Works on GUI widgets in the *Multi-script font features* window.
-**GUI Font size** - Applied dynamically to all GUI widgets in the *Multi-script font features* window.
-**Show widget font features** - Toggles **Multi-script font features** demonstration window with various widget types and text features.

For correct shaping of Arabic, Thai, and Hindi, the sample relies on a list of additional fonts set with *setGlobalFontFallback()*. The required Noto/Hind font files should be located in the `data/` folder. Missing fonts are rendered as placeholder boxes.