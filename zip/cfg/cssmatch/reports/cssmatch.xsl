<?xml version="1.0" encoding="ISO-8859-1"?>
<!--
	CSSMatch
	Nicolous
	http://code.google.com/p/cssmatch-plugin-en/
	
	Français ? German ? http://www.cssmatch.com/translations/
	
	The style.css and cssmatch.xsl are used to display the reports.
	To open the reports, open them with your Internet browser.
	
	These files don't need to stay on your game server. You can send them on your website. 
	All your reports have to be placed in the same folder than style.css and cssmatch.xsl to be displayed. 
	Once your reports and these files on your website, distribute the links corresponding to 
	your reports : everybody can see them :-)
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output 
	  method="html"
	  encoding="ISO-8859-1"
	  doctype-public="-//W3C//DTD HTML 4.01//EN"
	  doctype-system="http://www.w3.org/TR/html4/strict.dtd"
	  indent="yes"/>

	<!--======================= Racine =======================-->
	<xsl:template match="cssmatch">	
		<html>
			<head>
				<meta http-equiv="Content-Type" content="text/html;charset=utf-8"></meta>
				<title>Report "<xsl:value-of select="match/nom"/>"</title>
				<link rel="stylesheet" type="text/css" href="style.css"></link>
			</head>
			<body>
				<div class="sommaire">
					<xsl:call-template name="sommaire"/>
				</div>
				<div class="cssmatch">
					<div class="match">
						<xsl:apply-templates select="match"/>
					</div>
					<div class="plugin">
						<xsl:apply-templates select="plugin"/>
					</div>
				</div>
				<a href="#sommaire">Scroll up</a>
			</body>
		</html>
	</xsl:template>
	
	<!--======================= Sommaire de la page =======================-->
	<xsl:template name="sommaire">
		<a name="sommaire"><h1>Summary :</h1></a>
		<ul>
			<li><a href="#{match/nom}">Brief</a></li>
			<li><a href="#equipes">Teams</a></li>
			<li><a href="#participants">Members</a></li>
			<xsl:if test="match/sourcetv">
				<li><a href="#sourcetv">SourceTv records</a></li>
			</xsl:if>
			<li><a href="#plugin">About CSSMatch</a></li>
		</ul>
	</xsl:template>
	
	<!--======================= Balise "match" =======================-->
	<xsl:template match="match">
		<div class="match_entete">
			<a name="{nom}"><h1><xsl:value-of select="nom"/></h1></a>
			<xsl:value-of select="date"/>, <xsl:value-of select="debut"/> - <xsl:value-of select="fin"/><br/>
			Map : <xsl:value-of select="map"/> <br/>
		</div>
		<div class="teams">
			<xsl:apply-templates select="teams"/>
		</div>
		<div class="spectateurs">
			<xsl:for-each select="spectateurs">
					<xsl:call-template name="afficheSpectateurs"/>
			</xsl:for-each>
		</div>
		<div class="sourcetv">
			<xsl:apply-templates select="sourcetv"/>
		</div>
	</xsl:template>
	
	<!--======================= Balise "teams" =======================-->
	<xsl:template match="teams">
		<a name="equipes"><h1>Teams :</h1></a>
		<span class="note">Click on the team to see its players</span>
		<div class="team">
			<table>
				<tr>
					<td>Team name</td> <td>Final score</td> <td>Score in terrorist</td> <td>Score in counter-terrorist</td>
				</tr>
				<xsl:for-each select="team">
						<xsl:call-template name="afficheTeam"/>
				</xsl:for-each>
			</table>
			<span class="note"><sup>*</sup> Knife round winner (optionnal)</span>
			<xsl:if test="../spectateurs">
				<br/>	
				<span class="note">Some players were spectators at the end of the match, <a href="#spectateurs">click gere to see them</a></span>
			</xsl:if>
		</div>
		<a name="participants"><h1>Members :</h1></a>
		<div class="joueurs">
			<xsl:for-each select="team/joueurs">
					<xsl:call-template name="afficheJoueurs"/>
			</xsl:for-each>
		</div>
	</xsl:template>
	
	<!--======================= Balise "team" =======================-->
	<xsl:template name="afficheTeam">
		<tr>
			<td>
				<a href="#team{position()}"><xsl:value-of select="tag"/></a>
				<xsl:if test="tag = ../../tagcutround">
					<sup> *</sup>
				</xsl:if>
			</td>
			<td> <xsl:value-of select="score"/> </td>
			<td> <xsl:value-of select="scoret"/> </td>
			<td> <xsl:value-of select="scorect"/> </td>
		</tr>
	</xsl:template>
	
	<!--======================= Balise "joueurs" =======================-->
	<xsl:template name="afficheJoueurs">
		<h2><a name="team{position()}"><xsl:value-of select="../tag"/> players</a> :</h2> 
		<xsl:for-each select="joueur">
			<xsl:sort select="pseudo" order="ascending"/>
			<div class="joueur">
				<xsl:call-template name="afficheJoueur"/>
			</div>
		</xsl:for-each>
	</xsl:template>
	
	<!--======================= Balise "joueur" =======================-->
	<xsl:template name="afficheJoueur">
		<xsl:value-of select="pseudo"/> (<xsl:value-of select="@steamid"/>) <br/>
			<b>kills : </b> <xsl:value-of select="kills"/> <br/>
			<b>deaths : </b> <xsl:value-of select="deaths"/> <br/>
			<xsl:choose>
				<xsl:when test="deaths = 0">
					<b>ratio : </b> <xsl:value-of select="kills"/> <br/>
				</xsl:when>
				<xsl:otherwise>
					<b>ratio : </b> <xsl:value-of select="substring(kills div deaths,0,5)"/> <br/>
				</xsl:otherwise>
			</xsl:choose>
	</xsl:template>
	
	<!--======================= Balise "spectateurs" =======================-->
	<xsl:template name="afficheSpectateurs">
		<a name="spectateurs"><h1>Spectators :</h1></a>
		<xsl:for-each select="joueur">
			<xsl:sort select="pseudo" order="ascending"/>
			<div class="joueur">
				<xsl:call-template name="afficheJoueur"/>
			</div>
		</xsl:for-each>
	</xsl:template>
	
	<!--======================= Balise "sourcetv" =======================-->
	<xsl:template match="sourcetv">
		<a name="sourcetv"><h1>Name and localization of the SourceTv records :</h1></a>
		<xsl:for-each select="manche">
			<b>Set <xsl:value-of select="@numero"/> : </b> <xsl:value-of select="."/> <br/>
		</xsl:for-each>
	</xsl:template>
	
	<!--======================= Balise "plugin" =======================-->
	<xsl:template match="plugin">	
		<a name="plugin"><h1>CSSMatch :</h1></a>
		<b>Version : </b> <xsl:value-of select="version"/>
		<br/>
		<b>Web Site : </b> <a href="{url}"> <xsl:value-of select="url"/> </a> <br/>
		<br/>
	</xsl:template>
</xsl:stylesheet>
