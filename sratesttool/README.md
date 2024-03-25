# sratesttool
This is `sratesttool`. It is an introductory example of how to use the `libaxa`
API to connect to an SRA server and stream data.

Originally published in
[Farsight's Advanced Exchange Access: The C Programming API, Part One](https://www.domaintools.com/resources/user-guides/the-axa-api-reference/).

## sratesttool examples
An example using Farsight's Heartbeat channel (SIE channel 255):

    $ sratesttool -s tls:user@sraserver,1021 -c 255 -w ch=255
    Farsight Security SRA Test Tool
    connecting to tls:user@sraserver,1021...
    connected
    parsing watch: ch=255...
    parse ch=255 OK
    setting watch on server...
    watch set OK
    parsing channel: 255...
    parse 255 OK
    enabling channel on server...
    channel enabled OK
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:21.111788988
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:21.612488985
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:22.113173007
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:22.613782882
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:23.114434957
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:23.615050077
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:24.115665912
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:24.616344928
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:25.117007970
    NMSG watch hit, channel: 255 @ 2015-07-29T19:13:25.617640972
    ^C10 total watch hits
