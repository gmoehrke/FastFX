<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" encoding="iso-8859-1"/>
	
<xsl:strip-space elements="*"/>	
	
<xsl:template match="//compound/member[@kind='enum']">
<xsl:value-of select="name" /><xsl:text>&#x09;KEYWORD1&#x0D;&#x0A;</xsl:text>	
</xsl:template>	
	
<xsl:template match="//compound/member[@kind='enumvalue']">
<xsl:value-of select="name" /><xsl:text>&#x09;LITERAL1&#x0D;&#x0A;</xsl:text>		
</xsl:template>		
	
<xsl:template match="//compound/member[@kind='function']">	
		<xsl:if test="../name != ./name"> 
<xsl:value-of select="name" /><xsl:text>&#x09;KEYWORD2&#x0D;&#x0A;</xsl:text>
		</xsl:if>
</xsl:template>
	
<xsl:template match="//compound" />		
	
<xsl:template match="//compound[@kind='class']">
<xsl:value-of select="name" /><xsl:text>&#x09;KEYWORD1&#x0D;&#x0A;</xsl:text>
		<xsl:apply-templates select="member[@kind='function']" />
		<xsl:apply-templates select="member[@kind='enum']" />
		<xsl:apply-templates select="member[@kind='enumvalue']" />
</xsl:template>
	
	
<xsl:template match="/">
	<xsl:apply-templates/>
</xsl:template>
	
</xsl:stylesheet>