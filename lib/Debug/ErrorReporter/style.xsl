<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<html class="error-debug">
			<head>
				<title>
					<xsl:choose>
						<xsl:when test="/error">
							<xsl:value-of select="/error/message"/>
						</xsl:when>

						<xsl:when test="/environments">
							<xsl:text>Переменные окружения запроса</xsl:text>
						</xsl:when>
					</xsl:choose>
				</title>

				<style type="text/css">
					h1 {
						font-size: 1.5em;
					}

					.environments h1 {
						color: #ffffff;
						background-color: #cc3333;
						padding: 0.2em 0.5em;
					}

					.environments dl {
						padding: 2em;
					}

					.environments dt {
						font-weight: bold;
					}

					.environments dd {
						margin-top: 0.3em;
						margin-bottom: 1.5em;
					}

					.error-debug {
						color: #000000;
						background-color: #FFFFFF;
						font: normal normal small Consolas;
						padding: 1em;
					}

					div.error-debug {
						border: 3px solid #990000;
						margin: 1em 0;
					}

					table.error-trace {
						width: 100%;
						border-collapse: collapse;
						margin: 1em 0 0;
					}

					.error-debug-show_table table.error-trace {
						display: table;
					}

					table.error-trace th, table.error-trace td {
						border: 1px solid #000000;
						background-color: #FFFFFF;
						text-align: left;
						vertical-align: top;
						padding: 0.25em 0.5em;
					}

					table.error-trace td.line {
						text-align: right;
					}

					table.error-trace th {
						font-size: x-small;
						border: none;
					}

					table.error-trace span {
						font-size: x-small;
						color: #000000
					}

					.error-debug pre.desc {
						margin-left: 100px;
					}

					.error-debug .string  { color: #000066; }
					.error-debug .integer { color: #660000; }
					.error-debug .array   { color: #006600; }
					.error-debug .object  { color: #666600; }
					.error-debug .null    { color: #666666; }
				</style>
			</head>

			<body>
				<xsl:apply-templates select="/error"/>
				<xsl:apply-templates select="/environments/*"/>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="environments/*">
		<div class="environments">
			<h1>
				<xsl:text>$</xsl:text>
				<xsl:value-of select="local-name()"/>
			</h1>

			<dl>
				<xsl:for-each select="*">
					<dt>
						<xsl:value-of select="local-name()"/>
					</dt>

					<dd>
						<xsl:value-of select="."/>
					</dd>
				</xsl:for-each>
			</dl>
		</div>
	</xsl:template>


	<xsl:template match="/error">
		<h1>
			<xsl:value-of select="type"/>
		</h1>

		<p>
			<xsl:value-of select="message"/> в файле <strong><xsl:value-of select="file"/></strong> в строке <strong><xsl:value-of select="line"/></strong>
		</p>

		<table class="error-trace">
			<tr>
				<th width="25%">Файл:</th>
				<th width="10%">Строка:</th>
				<th>Метод:</th>
				<th>Аргументы:</th>
			</tr>

			<xsl:apply-templates select="trace/call" />
		</table>
	</xsl:template>

	<xsl:template match="call">
		<tr>
			<td>
				<xsl:value-of select="file"/>
			</td>

			<td class="line">
				<xsl:value-of select="line"/>
			</td>

			<td>
				<xsl:value-of select="method"/>
			</td>

			<td>
				<xsl:apply-templates select="arguments/*" />
			</td>
		</tr>
	</xsl:template>

	<xsl:template match="arguments/*">
		<span class="{local-name()}">
			<xsl:value-of select="."/>
		</span>

		<xsl:if test="position() &lt; last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>
