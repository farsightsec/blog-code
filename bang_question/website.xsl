<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:whois="https://www.arin.net/whoisrws/core/v1"
                xmlns:rdns="https://www.arin.net/whoisrws/rdns/v1"
                xmlns:pft="https://www.arin.net/whoisrws/pft/v1"
                version="1.0">
    <xsl:output method="html" doctype-system="https://www.w3.org/TR/html4/strict.dtd" doctype-public="-//W3C//DTD HTML 4.01//EN" indent="yes"/>

    <xsl:template match="/">
        
    <html>
    <head>
    <title>Whois-RWS</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta content="Virginia" name="geo.placename"/>
    <meta content="USA" name="geo.country"/>
    <meta name="viewport" content="width=990" />
    <meta name="Keywords" content="" />
    <meta name="Description" content="" />
    <style type="text/css">
       .pad_thai {

	      /* Extra yummy! */

	      padding-left: 2em;

	   }

      div#content tr.r0 td img, div#content tr.r1 td img
      {

         display:       block;
         margin-left:   0px !important;

      }

        div#maincontent table caption {
            background-color:#999999 !important;
            color:white;
            font-size:13px !important;
            padding:5px 10px !important;
            margin: 0 !important;
            text-transform:none !important;
            border-bottom-style: none !important;
        }

        div#maincontent table#delegation {
            table-layout:fixed;
            word-wrap: break-word;
            margin-top: 0;
        }

        div#maincontent table#delegation td p {
            margin: 0px;
            padding: 0.5em 0;
        }

        div#maincontent table#delegation td p:nth-of-type(even) {
           background-color: #D4EFFD;
        }
    </style>
<!-- InstanceEndEditable -->
	<!--styles-->
    <link href="https://whois.arin.net/ui/media/css/web.css" rel="stylesheet" type="text/css" media="screen" />
	<link href="https://whois.arin.net/ui/media/css/print_web.css" rel="stylesheet" type="text/css" media="print" />
	<link href="https://whois.arin.net/ui/media/css/handheld_web.css" rel="stylesheet" type="text/css" media="handheld" />
    <!-- InstanceBeginEditable name="form_style" --> <!-- InstanceEndEditable -->

    <base href="https://www.arin.net"/>

</head>
<body>
<div id="wrapper">
	<div id="header">
		<!-- Logo -->
        <h1><a href="https://www.arin.net"><acronym title="American Registry for Internet Numbers">ARIN</acronym></a></h1>

        <!-- Search box -->

        <fieldset id="search">
            <form action="https://whois.arin.net/ui/query.do" method="post" name="whois_query" id="whois_query">
                <label for="queryinput">Search <span class="casefix">Whois<i>RWS</i></span></label>
                <input type="hidden" name="xslt" value="https://localhost:8080/ui/arin.xsl"/>
                <input type="hidden" name="flushCache" value="false"/>
                <input type="text" id="queryinput" name="queryinput" />
                <span>
                    <input id="whoisSubmitButton" type="submit" name="whoisSubmitButton" value=" " />
                </span>
            </form>
            <span id="whoistos">All requests subject to <a href="https://www.arin.net/resources/registry/whois/tou/">terms of use</a></span>
            <span id="whoisadvanced"><a href="https://whois.arin.net/ui/advanced.jsp">Advanced Search</a></span>
        </fieldset>

	<!--End whois -->
</div>
<!-- End header --><!-- #BeginLibraryItem "/Library/sidebar.lbi" -->  <div id="sidebar">
  <div id="sidebar_login" style="display: none;"></div>
        <div id="secure">
            <h3>Secure Login</h3>
            <a href="https://account.arin.net/public/">enter</a>
        </div>

</div>
<!-- End sidebar --><!-- #EndLibraryItem --><div id="content">
  <h2 class="noauth"><!-- InstanceBeginEditable name="page_title" -->Whois-RWS <!-- InstanceEndEditable --></h2>
  <!-- #QUERY# -->
  <div id="maincontent">

<!-- InstanceBeginEditable name="Content" -->

    <p style="margin:0;padding:0;"><xsl:apply-templates/></p>

  <!-- InstanceEndEditable -->
</div>
  <div id="subcontent">


<!-- )@@" -->
<!-- InstanceBeginEditable name="right_nav_content" -->

 <div class="box">
  <h3>Relevant Links</h3>
  <ul>
    <li><a href="https://www.arin.net/resources/registry/whois/tou/">ARIN Whois/Whois-RWS Terms of Service</a></li>
    <li><a href="https://www.arin.net/resources/registry/whois/inaccuracy_reporting/">Report Whois Inaccuracy</a></li>
    <li><a href="https://search.arin.net/rdap/">Search ARIN Whois with RDAP</a></li>
  </ul>
 </div>

<!-- InstanceEndEditable -->
  </div>
  <!-- End right_nav -->
  </div><!-- End content -->
  <div style="clear: both;"></div>
  </div>
<!-- End wrapper -->

    <div id="footer">
        <ul>
            <li><a href="/contact/" title="Contact Us">Contact Us</a></li>
            <li><a href="https://account.arin.net/public/terms-of-service" title="Terms of Service">Terms of Service</a></li>
            <li><a href="/about/media/" title="Media">Media</a></li>
            <li><a href="/sitemap/" title="Site Map">Site Map</a></li>
            <li><a href="https://search.arin.net/" title="Search ARIN">Search ARIN</a></li>
            <li><a href="/about/privacy/" title="Privacy Statement">Privacy Statement</a></li>
            <li><a href="/reference/accessibility/" title="Accessibility">Accessibility</a></li>
            <li><a href="/reference/materials/abuse/" title="Network Abuse">Network Abuse</a></li>
        </ul>
        <p>By using the ARIN Whois service, you are agreeing to the  <a href="https://www.arin.net/resources/registry/whois/tou/" class="footer_link">Whois Terms of Use</a><br />
            &#169; Copyright 1997 - 2019, American Registry for Internet Numbers, Ltd.</p>
    </div>
</body>
</html>

    </xsl:template>

    <xsl:variable name="this">https://whois.arin.net/rest</xsl:variable>

    <!--
      *
      * Organization record template
      *
      -->
    <xsl:template match="whois:org">
    <table>
        <tr>
            <th colspan="2">Organization</th>
        </tr>
        <tr>
            <td>Name</td>
            <td><xsl:value-of select="whois:name"/></td>
        </tr>
        <tr>
            <td>Handle</td>
            <td><xsl:value-of select="whois:handle"/></td>
        </tr>
        <tr>
            <td>Street</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:streetAddress"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>City</td>
            <td><xsl:value-of select="whois:city"/></td>
        </tr>
        <tr>
            <td>State/Province</td>
            <td><xsl:value-of select="whois:iso3166-2" /></td>
        </tr>
        <tr>
            <td>Postal Code</td>
            <td><xsl:value-of select="whois:postalCode" /></td>
        </tr>
        <tr>
            <td>Country</td>
            <td><xsl:value-of select="whois:iso3166-1/whois:code2" /></td>
        </tr>
        <tr>
            <td>Registration Date</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:registrationDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Last Updated</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:updateDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Comments</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:comment"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
        </tr>
        <!--<xsl:for-each select="whois:orgUrls/whois:orgUrl">-->
        <!--<tr>-->
            <!--<td>Referral Server</td>-->
            <!--<td><xsl:value-of select="whois:url"/></td>-->
        <!--</tr>-->
        <!--</xsl:for-each>-->
        <xsl:choose>
        <xsl:when test="count(whois:nets/whois:netRef) > 0">
        <tr>
            <td colspan="2">
        <xsl:apply-templates select="whois:nets"/>
            </td>
        </tr>
        </xsl:when>
        <xsl:when test="count(/pft:pft) > 0">
        </xsl:when>
        <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/nets">Related networks.</a></td>
        </tr>
        </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
        <xsl:when test="count(whois:asns/whois:asnRef) > 0">
        <tr><td colspan="2">
        <xsl:apply-templates select="whois:asns"/>
        </td></tr>
        </xsl:when>
        <xsl:when test="count(/pft:pft) > 0">
        </xsl:when>
        <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/asns">Related autonomous system numbers.</a></td>
        </tr>
        </xsl:otherwise>
        </xsl:choose>

        <!--[CRT-50] Display ResourceLink listing link and/or display those links for PFT-->
        <xsl:if test="count(whois:resources/whois:resource) > 0">
            <xsl:choose>
                <xsl:when test="count(/pft:pft) > 0">
                    <tr>
                        <td colspan="2"><xsl:apply-templates select="whois:resources"/></td>
                    </tr>
                </xsl:when>
                <xsl:otherwise>
                    <tr>
                        <td>See Also</td>
                        <td><a href="{whois:ref}/resources">Resource links.</a></td>
                    </tr>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:if>

        <xsl:choose>
        <xsl:when test="count(whois:pocs/whois:pocLinkRef) > 0">
            <tr><td colspan="2">
        <xsl:apply-templates select="whois:pocs"/>
            </td></tr>
        </xsl:when>
        <xsl:when test="count(/pft:pft) > 0">
        </xsl:when>
        <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/pocs">Related POC records.</a></td>
        </tr>
        </xsl:otherwise>
        </xsl:choose>
    </table>
    <br/>
    <br/>


    </xsl:template>
    
    

    <!--
      * This is used to match lists of organization references.
      -->
    <xsl:template match="whois:orgs">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(whois:orgRef) > 0">
        <tr>
            <th colspan="2">Organizations</th>
        </tr>
        </xsl:if>
        <xsl:if test="count(whois:orgPocLinkRef) > 0">
        <tr>
            <th>Function</th>
            <th>Organization</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="whois:org"/>
        <xsl:apply-templates select="whois:orgRef"/>
        <xsl:apply-templates select="whois:orgPocLinkRef"/>
        </table>
    </xsl:template>

    <xsl:template match="whois:orgRef">
        <tr>
            <td>
                <xsl:value-of select="@name"/>
            </td>
            <td><a href="{$this}/org/{@handle}.html"><xsl:value-of select="@handle"/></a></td>
        </tr>
    </xsl:template>

    <xsl:template match="whois:orgPocLinkRef">
         <tr>
            <td><xsl:value-of select="@relPocDescription"/></td>
            <td>
                <xsl:value-of select="@name"/>
                (<a href="{$this}/org/{@handle}.html"><xsl:value-of select="@handle"/></a>)<br/>
            </td>
        </tr>
    </xsl:template>

    <!--
      *
      * POC record template
      *
      -->
    <xsl:template match="whois:poc" priority="-1">
        <table>
        <tr>
            <th colspan="2">Point of Contact</th>
        </tr>
        <xsl:if test="string(whois:note)">
            <tr>
                <td>Note</td>
                <td><xsl:value-of select="whois:note"/></td>
            </tr>
        </xsl:if>
        <tr>
            <td>Name</td>
            <td>
                <xsl:value-of select="whois:lastName"/>
                <xsl:if test="string(whois:firstName)">
                    , <xsl:value-of select="whois:firstName"/><xsl:text> </xsl:text><xsl:value-of select="whois:middleName"/>
                </xsl:if>
            </td>
        </tr>
        <tr>
            <td>Handle</td>
            <td><xsl:value-of select="whois:handle"/></td>
        </tr>
        <tr>
            <td>Company</td>
            <td><xsl:value-of select="whois:companyName"/></td>
        </tr>
        <tr>
            <td>Street</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:streetAddress"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>City</td>
            <td><xsl:value-of select="whois:city"/></td>
        </tr>
        <tr>
            <td>State/Province</td>
            <td><xsl:value-of select="whois:iso3166-2" /></td>
        </tr>
        <tr>
            <td>Postal Code</td>
            <td><xsl:value-of select="whois:postalCode" /></td>
        </tr>
        <tr>
            <td>Country</td>
            <td><xsl:value-of select="whois:iso3166-1/whois:code2" /></td>
        </tr>
        <tr>
            <td>Registration Date</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:registrationDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Last Updated</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:updateDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Comments</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:comment"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Phone</td>
            <td>
                <xsl:for-each select="whois:phones/whois:phone">
                    <xsl:value-of select="whois:number"/> (<xsl:value-of select="whois:type/whois:description"/>)<br/>
                </xsl:for-each>
            </td>
        </tr>
        <tr>
            <td>Email</td>
            <td>
                <xsl:for-each select="whois:emails/whois:email">
                    <xsl:value-of select="."/><br/>
                </xsl:for-each>
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
        </tr>
            <xsl:choose>
            <xsl:when test="count(whois:orgs/whois:orgPocLinkRef) > 0">
        <tr><td colspan="2">
            <xsl:apply-templates select="whois:orgs"/>
        </td></tr>
            </xsl:when>
            <xsl:when test="count(/pft:pft) > 0">
            </xsl:when>
            <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/orgs">Related organizations.</a></td>
        </tr>
            </xsl:otherwise>
            </xsl:choose>

            <!--[CRT-50] Display ResourceLink listing link and/or display those links for PFT-->
            <xsl:if test="count(whois:resources/whois:resource) > 0">
                <tr>
                    <td>See Also</td>
                    <td><a href="{whois:ref}/resources">Resource links.</a></td>
                </tr>
            </xsl:if>

    </table>
        <br/>
        <br/>


    </xsl:template>

    <!--
      * This is used to match lists of poc references.
      -->
    <xsl:template match="whois:pocs">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(whois:pocRef) > 0">
        <tr>
            <th colspan="2">Point of Contact</th>
        </tr>
        </xsl:if>
        <xsl:if test="count(whois:pocLinkRef) > 0">
        <tr>
            <th>Function</th>
            <th>Point of Contact</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="whois:poc"/>
        <xsl:apply-templates select="whois:pocRef"/>
        <xsl:apply-templates select="whois:pocLinkRef"/>
        </table>
    </xsl:template>

    <xsl:template match="whois:pocRef">
        <tr>
            <td>
                <xsl:value-of select="@name"/>
            </td>
            <td><a href="{$this}/poc/{@handle}.html"><xsl:value-of select="@handle"/></a></td>
        </tr>
    </xsl:template>

    <xsl:template match="whois:pocLinkRef">
        <tr>
            <td>
                <xsl:value-of select="@description"/>
            </td>
            <td><xsl:value-of select="@handle"/> (<a href="{$this}/poc/{@handle}.html"><xsl:value-of select="@handle"/></a>)</td>
        </tr>
    </xsl:template>

    <!--
      *
      * Customer record template
      *
      -->
    <xsl:template match="whois:customer" priority="-1">
    <table>
        <tr>
            <th colspan="2">Customer</th>
        </tr>
        <tr>
            <td>Name</td>
            <td><xsl:value-of select="whois:name"/></td>
        </tr>
        <tr>
            <td>Handle</td>
            <td><xsl:value-of select="whois:handle"/></td>
        </tr>
        <tr>
            <td>Street</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:streetAddress"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>City</td>
            <td><xsl:value-of select="whois:city"/></td>
        </tr>
        <tr>
            <td>State/Province</td>
            <td><xsl:value-of select="whois:iso3166-2" /></td>
        </tr>
        <tr>
            <td>Postal Code</td>
            <td><xsl:value-of select="whois:postalCode" /></td>
        </tr>
        <tr>
            <td>Country</td>
            <td><xsl:value-of select="whois:iso3166-1/whois:code2" /></td>
        </tr>
        <tr>
            <td>Registration Date</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:registrationDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Last Updated</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:updateDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Comments</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:comment"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
        </tr>
            <xsl:choose>
                <xsl:when test="count(/pft:pft) > 0">
                </xsl:when>
                <xsl:when test="count(whois:nets/whois:netRef) > 0">
                    <tr><td colspan="2">
                    <xsl:apply-templates select="whois:nets"/>
                    </td></tr>
                </xsl:when>
            <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/nets">Related networks.</a></td>
        </tr>
            </xsl:otherwise>
            </xsl:choose>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:nets/whois:netRef[1]}/parent/pocs">Upstream network's resource POC records.</a></td>
        </tr>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:parentOrgRef}/pocs">Upstream organization's POC records.</a></td>
        </tr>
    </table>
        <br/>
        <br/>


    </xsl:template>

    <!--
      * This is used to match lists of customers references.
      -->
    <xsl:template match="whois:customers">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(whois:customerRef) > 0">
        <tr>
            <th colspan="2">Customers</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="whois:customer"/>
        <xsl:apply-templates select="whois:customerRef"/>
        </table>
    </xsl:template>

    <xsl:template match="whois:customerRef">
        <tr>
            <td>
                <xsl:value-of select="@name"/>
            </td>
            <td><a href="{$this}/customer/{@handle}.html"><xsl:value-of select="@handle"/></a></td>
        </tr>
    </xsl:template>

    <!--
      *
      * ASN record template
      *
      -->
    <xsl:template match="whois:asn" priority="-1">
        <table>
        <tr>
            <th colspan="2">Autonomous System Number</th>
        </tr>
        <tr>
            <td>Number</td>
            <td>
                <xsl:value-of select="whois:startAsNumber"/>
                <xsl:if test="string(whois:endAsNumber) and whois:startAsNumber != whois:endAsNumber">
                 - <xsl:value-of select="whois:endAsNumber"/>
                </xsl:if>
            </td>
        </tr>
        <tr>
            <td>Name</td>
            <td><xsl:value-of select="whois:name"/></td>
        </tr>
        <tr>
            <td>Handle</td>
            <td><xsl:value-of select="whois:handle"/></td>
        </tr>
        <tr>
            <td>Organization</td>
            <td>
                <xsl:value-of select="whois:orgRef/@name" />
                (<a href="{$this}/org/{whois:orgRef/@handle}.html"><xsl:value-of select="whois:orgRef/@handle"/></a>)
            </td>
        </tr>
        <tr>
            <td>Registration Date</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:registrationDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Last Updated</td>
            <td>
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="whois:updateDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>Comments</td>
            <td>
                <xsl:call-template name="multiline">
                    <xsl:with-param name="node" select="whois:comment"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
        </tr>
            <xsl:choose>
            <xsl:when test="count(whois:pocs/whois:pocLinkRef) > 0">
                <tr><td colspan="2">
            <xsl:apply-templates select="whois:pocs"/>
                </td></tr>
            </xsl:when>
            <xsl:when test="count(/pft:pft) > 0">
            </xsl:when>
            <xsl:otherwise>
        <tr>
            <td>See Also</td>
            <td><a href="{whois:ref}/pocs">Related POC records.</a></td>
        </tr>
            </xsl:otherwise>
            </xsl:choose>

            <!--[CRT-50] Display ResourceLink listing link and/or display those links for PFT-->
            <xsl:if test="count(whois:resources/whois:resource) > 0">
                <tr>
                    <td>See Also</td>
                    <td><a href="{whois:ref}/resources">Resource links.</a></td>
                </tr>
            </xsl:if>

        <tr>
            <td>See Also</td>
            <td><a href="{$this}/org/{whois:orgRef/@handle}/pocs">Organization's POC records.</a></td>
        </tr>
    </table>
        <br/>
        <br/>


    </xsl:template>


    <!--
      * This is used to match lists of asn references.
      -->
    <xsl:template match="whois:asns">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(whois:asnRef) > 0">
        <tr>
            <th colspan="2">Autonomous System Numbers</th>
        </tr>
        </xsl:if>
        <xsl:if test="count(whois:asnPocLinkRef) > 0">
        <tr>
            <th>Function</th>
            <th>ASN</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="whois:asn"/>
        <xsl:apply-templates select="whois:asnRef"/>
        <xsl:apply-templates select="whois:asnPocLinkRef"/>
        </table>
    </xsl:template>

    <xsl:template match="whois:asnRef">
        <tr>
            <td>
                <xsl:value-of select="@name"/>
            </td>
            <td><xsl:value-of select="@handle"/> (<a href="{$this}/asn/{@handle}.html"><xsl:value-of select="@handle"/></a>)</td>
        </tr>
    </xsl:template>

    <xsl:template match="whois:asnPocLinkRef">
        <tr>
            <td><xsl:value-of select="@relPocDescription"/></td>
            <td>
                <xsl:value-of select="@name"/>
                (<a href="{$this}/asn/{@handle}.html"><xsl:value-of select="@handle"/></a>)<br/>
            </td>
        </tr>
    </xsl:template>

    <!--
      *
      * NET record template
      *
      -->
    <xsl:template match="whois:net" priority="-1">
        <table>
            <tr>
                <th colspan="2">Network</th>
            </tr>
            <tr>
                <td>Net Range</td>
                <td><xsl:value-of select="whois:startAddress" /> - <xsl:value-of select="whois:endAddress" /></td>
            </tr>
            <tr>
                <td>CIDR</td>
                <td><xsl:for-each select="whois:netBlocks/whois:netBlock"><xsl:sort select="whois:startAddress"/><xsl:value-of select="whois:startAddress" />/<xsl:value-of select="whois:cidrLength" /><br/></xsl:for-each></td>
            </tr>
            <tr>
                <td>Name</td>
                <td><xsl:value-of select="whois:name"/></td>
            </tr>
            <tr>
                <td>Handle</td>
                <td><xsl:value-of select="whois:handle"/></td>
            </tr>
            <tr>
                <td>Parent</td>
                <td>
                    <xsl:value-of select="whois:parentNetRef/@name" />
                    <xsl:if test="string(whois:parentNetRef/@handle)">
                    (<a href="{$this}/net/{whois:parentNetRef/@handle}.html"><xsl:value-of select="whois:parentNetRef/@handle"/></a>)
                    </xsl:if>
                </td>
            </tr>
            <tr>
                <td>Net Type</td>
                <td><xsl:value-of select="whois:netBlocks/whois:netBlock/whois:description" /></td>
            </tr>
            <tr>
                <td>Origin AS</td>
                <td>
                    <xsl:for-each select="whois:originASes/whois:originAS">
                        <xsl:value-of select="."/>
                        <xsl:if test="position()!=last()"><br/></xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
            <xsl:if test="count(whois:nameservers) > 0">
                <tr>
                    <td>Nameservers</td>
                    <td>
                        <xsl:for-each select="whois:nameservers/whois:nameserver">
                            <xsl:value-of select="."/>
                            <xsl:if test="position()!=last()"><br/></xsl:if>
                        </xsl:for-each>
                    </td>
                </tr>
            </xsl:if>
            <xsl:if test="count(whois:orgRef) > 0">
                <tr>
                    <td>Organization</td>
                    <td>
                        <xsl:value-of select="whois:orgRef/@name" />
                        (<a href="{$this}/org/{whois:orgRef/@handle}.html"><xsl:value-of select="whois:orgRef/@handle"/></a>)
                    </td>
                </tr>
            </xsl:if>
            <xsl:if test="count(whois:customerRef) > 0">
                <tr>
                    <td>Customer</td>
                    <td>
                        <xsl:value-of select="whois:customerRef/@name" />
                        (<a href="{$this}/customer/{whois:customerRef/@handle}.html"><xsl:value-of select="whois:customerRef/@handle"/></a>)
                    </td>
                </tr>
            </xsl:if>
            <tr>
                <td>Registration Date</td>
                <td>
                    <xsl:call-template name="date">
                        <xsl:with-param name="date" select="whois:registrationDate"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td>Last Updated</td>
                <td>
                    <xsl:call-template name="date">
                        <xsl:with-param name="date" select="whois:updateDate"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td>Comments</td>
                <td>
                    <xsl:call-template name="multiline">
                        <xsl:with-param name="node" select="whois:comment"/>
                    </xsl:call-template>
                </td>
            </tr>
            <tr>
                <td>RESTful Link</td>
                <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
            </tr>
            <xsl:if test="count(whois:orgRef) > 0">
                <xsl:choose>
                <xsl:when test="count(whois:pocs/whois:pocLinkRef) > 0">
                    <tr><td colspan="2">
                <xsl:apply-templates select="whois:pocs"/>
                    </td></tr>
                </xsl:when>
                <xsl:when test="count(/pft:pft) > 0">
                </xsl:when>
                <xsl:otherwise>
            <tr>
                <td>See Also</td>
                <td><a href="{whois:ref}/pocs">Related POC records.</a></td>
            </tr>
                </xsl:otherwise>
                </xsl:choose>

            <tr>
                <td>See Also</td>
                <td><a href="{$this}/org/{whois:orgRef/@handle}/pocs">Related organization's POC records.</a></td>
            </tr>
            </xsl:if>

            <!--[CRT-50] Display ResourceLink listing link and/or display those links for PFT-->
            <xsl:if test="count(whois:resources/whois:resource) > 0">
                <tr>
                    <td>See Also</td>
                    <td><a href="{whois:ref}/resources">Resource links.</a></td>
                </tr>
            </xsl:if>

            <xsl:if test="count(whois:customerRef) > 0">
            <tr>
                <td>See Also</td>
                <td><a href="{whois:parentNetRef}/pocs">Upstream network's resource POC records.</a></td>
            </tr>
            <tr>
                <td>See Also</td>
                <td><a href="{whois:parentNetRef}/org/pocs">Upstream organization's POC records.</a></td>
            </tr>
            </xsl:if>
            <tr>
                <td>See Also</td>
                <td><a href="{whois:ref}/rdns">Related delegations.</a></td>
            </tr>
        </table>
        <br/>
        <br/>


    </xsl:template>

    <xsl:template match="whois:resources">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <xsl:if test="count(whois:resource) > 0">
        <table>
            <tr>
                <th colspan="2">Resources</th>
            </tr>
            <xsl:for-each select="whois:resource">
                <tr>
                    <td><a>
                        <xsl:attribute name="href">
                            <xsl:value-of select="@url"/>
                        </xsl:attribute>
                        <xsl:value-of select="@url"/>
                    </a></td>
                    <td><xsl:value-of select="@description"/></td>
                </tr>
            </xsl:for-each>
        </table>
        </xsl:if>
    </xsl:template>

    <!--
          Used to match a list of Delegations refs.
    -->
    <xsl:template match="rdns:delegations">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(rdns:delegationRef) > 0">
        <tr>
            <th colspan="2">Network Delegation</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="rdns:delegation"/>
        <xsl:apply-templates select="rdns:delegationRef"/>
        </table>
    </xsl:template>

    <xsl:template match="rdns:delegationRef">
        <tr>
            <td>
                <a href="{$this}/rdns/{@name}"><xsl:value-of select="@name"/></a>
            </td>
        </tr>
    </xsl:template>
    
    <!--
      * This is used to match lists of net references.
      -->
    <xsl:template match="whois:nets">
        <xsl:apply-templates select="whois:limitExceeded"/>
        <table>
        <xsl:if test="count(whois:netRef) > 0">
        <tr>
            <th colspan="2">Network Resources</th>
        </tr>
        </xsl:if>
        <xsl:if test="count(whois:netPocLinkRef) > 0">
        <tr>
            <th>Function</th>
            <th>Network Resource</th>
        </tr>
        </xsl:if>
        <xsl:apply-templates select="whois:net"/>
        <xsl:apply-templates select="whois:netRef"/>
        <xsl:apply-templates select="whois:netPocLinkRef"/>
        </table>
    </xsl:template>

    <xsl:template match="whois:netRef">
        <tr>
            <td>
                <xsl:value-of select="@name"/>
                (<a href="{$this}/net/{@handle}.html"><xsl:value-of select="@handle"/></a>)
            </td>
            <td>
                <xsl:value-of select="@startAddress"/> - <xsl:value-of select="@endAddress"/>
            </td>
        </tr>
    </xsl:template>

    <xsl:template match="whois:netPocLinkRef">
        <tr>
            <td>
                <xsl:value-of select="@relPocDescription"/>
            </td>
            <td><xsl:value-of select="@name"/> (<a href="{$this}/net/{@handle}.html"><xsl:value-of select="@handle"/></a>)</td>
        </tr>
    </xsl:template>

    <!--
      *
      * NETBLOCK record template
      *
      -->
    <xsl:template match="whois:netBlock" priority="-1">
    <table>
        <tr>
            <th colspan="2">Network Block</th>
        </tr>
        <tr>
            <td>Net Range</td>
            <td><xsl:value-of select="whois:addressRange/whois:startAddress" /> - <xsl:value-of select="whois:addressRange/whois:endAddress" /></td>
        </tr>
        <tr>
            <td>CIDR</td>
            <td><xsl:value-of select="whois:addressRange/whois:startAddress" />/<xsl:value-of select="whois:addressRange/whois:cidrLength" /></td>
        </tr>
        <tr>
            <td>Net Type</td>
            <td><xsl:value-of select="whois:description"/></td>
        </tr>
        <tr>
            <td>Network</td>
            <td>
                <xsl:value-of select="whois:netRef/@name"/> (<a href="{whois:netRef}"><xsl:value-of select="whois:netRef/@handle"/></a>)
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{whois:ref}"><xsl:value-of select="whois:ref"/></a></td>
        </tr>
    </table>
    </xsl:template>

    <!--
      *
      * Delegation record template
      *
      -->
    <xsl:template match="rdns:delegation" priority="-1">
    <table id="delegation">
        <th colspan="2">Delegation</th>
        <tr>
            <td style="width:100px">Name</td>
            <td id="delnametd"><xsl:value-of select="rdns:name"/></td>
        </tr>
        <xsl:if test="count(rdns:nameservers) > 0">
            <tr>
                <td>Nameservers</td>
                <td id="nstd">
                    <xsl:for-each select="rdns:nameservers/rdns:nameserver">
                        <xsl:value-of select="."/>
                        <xsl:if test="position()!=last()"><br/></xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
        </xsl:if>
        <xsl:if test="count(rdns:delegationKeys) > 0">
            <tr>
                <td>Delegation Signer Keys</td>
                <td id="delkeytd">
                    <xsl:for-each select="rdns:delegationKeys/rdns:delegationKey">
                        DS <xsl:value-of select="rdns:keyTag"/><xsl:text> </xsl:text><xsl:value-of select="rdns:algorithm"/><xsl:text> </xsl:text>
                        <xsl:value-of select="rdns:digestType"/><xsl:text> </xsl:text><xsl:value-of select="rdns:digest"/>
                          <xsl:if test="position()!=last()"><br/></xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
        </xsl:if>
        <tr>
            <td>Last Updated</td>
            <td id="updatetd">
                <xsl:call-template name="date">
                    <xsl:with-param name="date" select="rdns:updateDate"/>
                </xsl:call-template>
            </td>
        </tr>
        <tr>
            <td>RESTful Link</td>
            <td><a href="{rdns:ref}" id="ref"><xsl:value-of select="rdns:ref"/></a></td>
        </tr>
        <tr>
            <td>See Also</td>
            <td><a href="{rdns:ref}/nets" id="nets">Networks with authority over this delegation.</a></td>
        </tr>
    </table>
        <br/>
        <br/>

  
    </xsl:template>


    <!--
      *
      * No search results template
      *
      -->
    <xsl:template match="noSearchResults" priority="-1">
    <table>
    <tr>
        <th>No Search Results</th>
    </tr>
    <tr>
        <td>Your search did not yield any results.</td>
    </tr>
    </table>
    </xsl:template>

    <!--
      *
      * No record found template
      *
      -->
    <xsl:template match="noRecord" priority="-1">
        <table>
            <tr>
                <th>No Record Found</th>
            </tr>
            <tr>
                <td>No record found for the handle provided.</td>
            </tr>
        </table>
    </xsl:template>

    <!--
      *
      * No related resources found template
      *
      -->
    <xsl:template match="noRelatedResources" priority="-1">
        <table>
            <tr>
                <th>No Related Resources</th>
            </tr>
            <tr>
                <td>No related resources were found for the handle provided.</td>
            </tr>
        </table>
    </xsl:template>

    <!--
      *
      * Bad request template
      *
      -->
    <xsl:template match="badRequest | methodNotAllowed" priority="-1">
        <table>
            <tr>
                <th>Bad Request</th>
            </tr>
            <tr>
                <td>Your request could not be processed because it is not composed correctly.</td>
            </tr>
        </table>
    </xsl:template>

    <!--
      *
      * HTTP 406 error template
      *
      -->
    <xsl:template match="notAcceptable" priority="-1">
        <table>
            <tr>
                <th>Error - HTTP Status 406</th>
            </tr>
            <tr>
                <td>The resource identified by this request is only capable of generating responses with characteristics
                    not acceptable according to the request "accept" headers (No match for accept header).</td>
            </tr>
        </table>
    </xsl:template>

    <!--
      *
      * Internal server error template
      *
      -->
    <xsl:template match="whois:internalServerError" priority="-1">
        <table>
            <tr>
                <th>Internal Server Error</th>
            </tr>
            <tr>
                <td>Your request could not be processed due to an error.<br/><br/>
                    To report this error, please document the steps you took before the error occurred and e-mail it to <a href="mailto:websupport@arin.net">websupport@arin.net</a>.
                </td>
            </tr>
        </table>
    </xsl:template>

    <xsl:template match="whois:limitExceeded">
    <xsl:if test=".='true'">
    <b>This list contains more than <xsl:value-of select="@limit"/> records.
    Additional records are not shown.</b><br/>
    <br/>
    </xsl:if>
    </xsl:template>

    <xsl:template name="date">
        <xsl:param name="date" />
        <xsl:value-of select="substring($date, 1, 10)" />
    </xsl:template>

    <xsl:template name="multiline">
        <xsl:param name="node" />
        <xsl:for-each select="$node/whois:line">
            <xsl:value-of select="."/><br/>
        </xsl:for-each>
    </xsl:template>


</xsl:stylesheet>
