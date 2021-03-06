//------------------------------------------------------------------------------
// <copyright file="HttpCapabilitiesSectionHandler.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

//
// ConfigureCapabilities is a parser CapabilitiesEvaluator objects;
// it's driven by the configuration system.
//
//

namespace System.Web.Configuration {

    using System.Collections;
    using System.Configuration;
    using System.IO;
    using System.Security;
    using System.Security.Permissions;
    using System.Text.RegularExpressions;
    using System.Web.Configuration;
    using System.Web.Util;
    using System.Xml;

    using Pair = System.Web.UI.Pair;

    //
    // ConfigureCapabilities is used to configure the CapabilitiesEvaluator object
    //
    internal class HttpCapabilitiesSectionHandler : IConfigurationSectionHandler {
        
        internal HttpCapabilitiesSectionHandler() {
        }
        

        class ParseState {
            internal string SectionName;
            internal HttpCapabilitiesEvaluator Evaluator;
            internal ArrayList RuleList = new ArrayList();
            internal ArrayList FileList = new ArrayList();
            internal bool IsExternalFile = false;
            
            internal ParseState() {}
        }
            

        //
        // As required by IConfigurationSectionHandler
        //
        public object Create(object parent, object configurationContext, XmlNode section) {
            // if called through client config don't even load HttpRuntime
            if (!HandlerBase.IsServerConfiguration(configurationContext))
                return null;

            ParseState parseState = new ParseState();
            parseState.SectionName = section.Name;

            // the rule is going to be the previous rule followed by a list containing the new rules
            parseState.Evaluator = new HttpCapabilitiesEvaluator((HttpCapabilitiesEvaluator)parent);
            ArrayList rulelist = new ArrayList();

            // check for random attributes
            
            HandlerBase.CheckForUnrecognizedAttributes(section);


            // iterate through XML section in order and apply the directives

            ArrayList sublist;

            sublist = RuleListFromElement(parseState, section, true);

            if (sublist.Count > 0) {
                parseState.RuleList.Add(new CapabilitiesSection(CapabilitiesRule.Filter, null, null, sublist));
            }

            if (parseState.FileList.Count > 0) {
                parseState.IsExternalFile = true;
                ResolveFiles(parseState, configurationContext);
            }
            
            // Add the new rules

            parseState.Evaluator.AddRuleList(parseState.RuleList);

            return parseState.Evaluator;
        }

        //
        // Create a rule from an element
        //
        static CapabilitiesRule RuleFromElement(ParseState parseState, XmlNode element) {
            int ruletype;
            DelayedRegex regex;
            CapabilitiesPattern pat;

            // grab tag name

            if (element.Name == "filter") {
                ruletype = CapabilitiesRule.Filter;
            }
            else if (element.Name=="case") {
                ruletype = CapabilitiesRule.Case;
            }
            else if (element.Name == "use") {
                HandlerBase.CheckForChildNodes(element);

                string var = HandlerBase.RemoveRequiredAttribute(element, "var");
                string strAs = HandlerBase.RemoveAttribute(element, "as");
                HandlerBase.CheckForUnrecognizedAttributes(element);

                if (strAs == null)
                    strAs = "";

                parseState.Evaluator.AddDependency(var);

                return new CapabilitiesUse(var, strAs);
            }
            else {
                throw new ConfigurationException(
                                                HttpRuntime.FormatResourceString(SR.Unknown_tag_in_caps_config, element.Name), 
                                                element);
            }

            // grab attributes
            String matchpat = HandlerBase.RemoveAttribute(element, "match");
            String testpat = HandlerBase.RemoveAttribute(element, "with");
            HandlerBase.CheckForUnrecognizedAttributes(element);

            if (matchpat == null) {
                if (testpat != null)
                    throw new ConfigurationException(HttpRuntime.FormatResourceString(SR.Cannot_specify_test_without_match), element);
                regex = null;
                pat = null;
            }
            else {
                try {
                    regex = new DelayedRegex(matchpat);
                }
                catch (Exception e) {
                    throw new ConfigurationException(e.Message, e, element);
                }

                if (testpat == null)
                    pat = CapabilitiesPattern.Default;
                else
                    pat = new CapabilitiesPattern(testpat);
            }

            // grab contents
            ArrayList subrules = RuleListFromElement(parseState, element, false);

            return new CapabilitiesSection(ruletype, regex, pat, subrules);
        }

        //
        // Create a rulelist from an element's children
        //
        static ArrayList RuleListFromElement(ParseState parseState, XmlNode node, bool top) {
            ArrayList result = new ArrayList();

            foreach (XmlNode child in node.ChildNodes) {
                switch (child.NodeType) {
                    case XmlNodeType.Text:
                    case XmlNodeType.CDATA:
                        top = false;
                        AppendLines(result, child.Value, node);
                        break;

                    case XmlNodeType.Element:
                        switch (child.Name) {
                            case "result":
                                if (top) {
                                    ProcessResult(parseState.Evaluator, child);
                                }
                                else {
                                    throw new ConfigurationException(
                                                    HttpRuntime.FormatResourceString(SR.Result_must_be_at_the_top_browser_section), 
                                                    child);
                                }
                                break;
                                
                            case "file": 
                                if (parseState.IsExternalFile) {
                                    throw new ConfigurationException(
                                                    HttpRuntime.FormatResourceString(SR.File_element_only_valid_in_config),
                                                    child);
                                }
                                ProcessFile(parseState.FileList, child);
                                break;

                            default: 
                                result.Add(RuleFromElement(parseState, child));
                                break;
                        }
                        top = false;
                        break;

                    case XmlNodeType.Comment:
                    case XmlNodeType.Whitespace:
                        break;

                    default:
                        HandlerBase.ThrowUnrecognizedElement(child);
                        break;
                }
            }

            return result;
        }


        //
        // Handle <file src=""/> elements
        //
        static void ProcessFile(ArrayList fileList, XmlNode node) {
            
            string filename = null;
            XmlNode attr = HandlerBase.GetAndRemoveRequiredStringAttribute(node, "src", ref filename);

            HandlerBase.CheckForUnrecognizedAttributes(node);
            HandlerBase.CheckForChildNodes(node);

            fileList.Add(new Pair(filename, attr));
        }


        //
        // Handle the <result> tag
        //
        static void ProcessResult(HttpCapabilitiesEvaluator capabilitiesEvaluator, XmlNode node) {

            bool inherit = true;
            HandlerBase.GetAndRemoveBooleanAttribute(node, "inherit", ref inherit);
            if (inherit == false) {
                capabilitiesEvaluator.ClearParent();
            }

            Type resultType = null;
            XmlNode attribute = HandlerBase.GetAndRemoveTypeAttribute(node, "type", ref resultType);
            if (attribute != null) {
                if (resultType.Equals(capabilitiesEvaluator._resultType) == false) {
                    // be sure the new type is assignable to the parent type
                    HandlerBase.CheckAssignableType(attribute, capabilitiesEvaluator._resultType, resultType);
                    capabilitiesEvaluator._resultType = resultType;
                }
            }

            int cacheTime = 0;
            attribute = HandlerBase.GetAndRemovePositiveIntegerAttribute(node, "cacheTime", ref cacheTime);
            if (attribute != null) {
                capabilitiesEvaluator.SetCacheTime(cacheTime);
            }

            HandlerBase.GetAndRemoveBooleanAttribute(node, "cache", ref capabilitiesEvaluator._useCache);

            HandlerBase.CheckForUnrecognizedAttributes(node);
            HandlerBase.CheckForChildNodes(node);

        }


        // 
        // ResolveFiles - parse files referenced with <file src="" />
        //
        static void ResolveFiles(ParseState parseState, object configurationContext) {

            //
            // 1) get the directory of the configuration file currently being parsed
            //

            HttpConfigurationContext httpConfigurationContext = (HttpConfigurationContext) configurationContext;
            string configurationDirectory = null;
            bool useAssert = false;

            //
            // Only assert to read cap files when parsing machine.config 
            // (allow device updates to work in restricted trust levels).
            //
            // Machine.config can be securely identified by the context being 
            // an HttpConfigurationContext with null path.
            //
            try {
                if (httpConfigurationContext.VirtualPath == null) {
                    useAssert = true;
                    // we need to assert here to get the file path from ConfigurationException
                    FileIOPermission fiop = new FileIOPermission(PermissionState.None);
                    fiop.AllFiles = FileIOPermissionAccess.PathDiscovery;
                    fiop.Assert();
                }
                
                Pair pair0 = (Pair)parseState.FileList[0];
                XmlNode srcAttribute = (XmlNode)pair0.Second;
                configurationDirectory = Path.GetDirectoryName(ConfigurationException.GetXmlNodeFilename(srcAttribute));
            }
            finally {
                if (useAssert) {
                    CodeAccessPermission.RevertAssert();
                }
            }

            //
            // 2) iterate through list of referenced files, builing rule lists for each
            //
            foreach (Pair pair in parseState.FileList) {
                string srcFilename = (string)pair.First;
                string fullFilename = Path.Combine(configurationDirectory, srcFilename);

                XmlNode section;
                try {
                    if (useAssert) {
                        InternalSecurityPermissions.FileReadAccess(fullFilename).Assert();
                    }

                    Exception fcmException = null;
                    
                    try {
                        HttpConfigurationSystem.AddFileDependency(fullFilename);
                    }
                    catch (Exception e) {
                        fcmException = e;
                    }

                    ConfigXmlDocument configDoc = new ConfigXmlDocument();
                    
                    try {
                        configDoc.Load(fullFilename);
                        section = configDoc.DocumentElement;
                    }
                    catch (Exception e) {
                        throw new ConfigurationException(HttpRuntime.FormatResourceString(SR.Error_loading_XML_file, fullFilename, e.Message), 
                                        e, (XmlNode)pair.Second);
                    }

                    if (fcmException != null)
                        throw fcmException;
                }
                finally {
                    if (useAssert) {
                        // Cannot apply next FileReadAccess PermissionSet unless 
                        // current set is explicitly reverted.  Also minimizes
                        // granted permissions.
                        CodeAccessPermission.RevertAssert();
                    }
                }
                
                if (section.Name != parseState.SectionName) {
                    throw new ConfigurationException(HttpRuntime.FormatResourceString(SR.Capability_file_root_element, parseState.SectionName), 
                                    section);
                }
                    
                HandlerBase.CheckForUnrecognizedAttributes(section);

                ArrayList sublist = RuleListFromElement(parseState, section, true);

                if (sublist.Count > 0) {
                    parseState.RuleList.Add(new CapabilitiesSection(CapabilitiesRule.Filter, null, null, sublist));
                }
            }
        }


        //
        // Some line counting
        //
        static int LineCount(String text, int offset, int newoffset) {
            int linecount = 0;

            if (newoffset < offset)
                throw new ArgumentException(HttpRuntime.FormatResourceString(SR.Cant_count_lines_backwards));

            while (offset < newoffset) {
                if (text[offset] == '\r' || (text[offset] == '\n' && (offset == 0 || text[offset - 1] != '\r')))
                    linecount++;
                offset++;
            }

            return linecount;
        }

        //
        // Regex dealing with a=b lines
        //
        static Regex lineRegex = new Regex
                                          (
                                          "\\G" +                                 // anchored
                                          "(?<var>\\w+)" +                        // variable name
                                          "\\s*=\\s*" +                           // equals sign
                                          "(?:" +
                                          "\"(?<pat>[^\"\r\n\\\\]*" +         // quoted pattern
                                          "(?:\\\\.[^\"\r\n\\\\]*)*)\"" +
                                          "|(?!\")(?<pat>\\S+)" +             // unquoted pattern
                                          ")" +
                                          "\\s*"                                  // trailing whitespace
                                          );

        static Regex wsRegex = new Regex("\\G\\s*");
        static Regex errRegex = new Regex("\\G\\S {0,8}");

        static void AppendLines(ArrayList setlist, String text, XmlNode node) {
            int lineNumber = ConfigurationException.GetXmlNodeLineNumber(node);
            int textpos;

            textpos = 0;

            for (;;) {
                Match match;

                if ((match = wsRegex.Match(text, textpos)).Success) {
                    lineNumber += LineCount(text, textpos, match.Index + match.Length);
                    textpos = match.Index + match.Length;
                }

                if (textpos == text.Length)
                    break;

                if ((match = lineRegex.Match(text, textpos)).Success) {
                    setlist.Add(new CapabilitiesAssignment(match.Groups["var"].Value,
                                                           new CapabilitiesPattern(match.Groups["pat"].Value)));

                    lineNumber += LineCount(text, textpos, match.Index + match.Length);
                    textpos = match.Index + match.Length;
                }
                else {
                    match = errRegex.Match(text, textpos);

                    throw new ConfigurationException(
                                    HttpRuntime.FormatResourceString(SR.Problem_reading_caps_config, match.ToString()), 
                                    ConfigurationException.GetXmlNodeFilename(node),
                                    lineNumber);
                }
            }
        }

    }

    internal class DelayedRegex {

        private String _regstring;
        private Regex _regex;

        internal DelayedRegex(String s) {
            _regex = null;
            _regstring = s;
        }

        internal Match Match(String s) {
            EnsureRegex();
            return _regex.Match(s);
        }

        internal int GroupNumberFromName(String name) {
            EnsureRegex();
            return _regex.GroupNumberFromName(name);
        } 

        internal void EnsureRegex() {
            string regstring = _regstring;
            if(_regex == null) {
                _regex = new Regex(regstring);
                //free original
                _regstring = null;
            }
            return;
        }
    }
}
