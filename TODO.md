# To do
- Make it so that 'execute' would be able to execute things with arguments.
- Add brief explanations under shebangs.
- Integrate -pc option of dmenu into usable scripts.
- Make getfl, getloc and their derivatives into a hash table with files in some hard-coded directory as initial containers and lines in the files being the contained elements. Then, search through the files (in case there is a hash collision) for the target. This should make searching for the path of a given file/location code a bit faster and more importantly make it roughly O(n).
- Create software alternatives to the bright\* scripts that work on hardware level. Use `xr --output "$( mondef )" --brightness "$brightness"`. Create a more general version that automatically uses the hardware one if supported on hardware, else use the software one.
- Make statbar elements write to a file instead of print to stdout. This will allow for caching, etc. and prevent high delays while refreshing, especially due to things like statbarvpn. Update some elements periodically, some elements on every nth refresh where n is a non-negative integer.
- After every usage of getloc, getfl, etc., add [ -z "$string" ] && exit 1.
