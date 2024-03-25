# nmsgjsonseqsrc
A simple example of how to use the
[`nmsg`](https://github.com/farsightsec/nmsg) IO engine, callback interface,
and seqsrc container tracking feature set. It listens for NMSGs on the network
and emits them as newline delimited JSON blobs. When the user quits via
ctrl-c, nmsgjsonseqsrc prints the number of containers received / dropped.
Originally published in [Farsight Security's Network Message: Loss Tracking Explained](https://www.domaintools.com/resources/blog/farsights-network-message-loss-tracking-explained/).

## nmsgjsonseqsrc example
To wit (after starting nmsgjsonseqsrc, run something like "nmsgtool -r foo.nmsg -s 127.0.0.1,8888" in another window):

    $ ./nmsgjsonseqsrc -l 127.0.0.1/8888
    nmsg JSON emitter / seqsrc example program
    nmsg initialized
    nmsg io engine initialized
    nmsg socket input initialized
    verify seqsrc enabled
    socket input added to io engine
    callback initialized
    callback added to io engine
    entering io loop, <ctrl-c> to quit...
    {"time":"2014-08-12 22:11:01.130599975","vname":"SIE","mname":"newdomain","source":"a1ba02cf","message":{"domain":"relaxlah.my.","time_seen":"2014-08-12 22:08:22","rrname":"relaxlah.my.","rrclass":"IN","rrtype":"MX","rdata":["0 relaxlah.my."],"keys":[],"new_rr":[]}}
    {"time":"2014-08-12 22:11:01.741137981","vname":"SIE","mname":"newdomain","source":"a1ba02cf","message":{"domain":"brainsciencetoday.com.","time_seen":"2014-08-12 22:08:47","rrname":"brainsciencetoday.com.","rrclass":"IN","rrtype":"NS","rdata":["ns1.monikerdns.net.","ns2.monikerdns.net.","ns3.monikerdns.net.","ns4.monikerdns.net."],"keys":[],"new_rr":[]}}
    ^C
    recevied 1 containers
    dropped 0 containers
