//------------------------------------------------------------------------------
// <copyright file="ServiceDescription.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.Services.Description {

    using System.Xml.Serialization;
    using System.Xml.Schema;
    using System.Collections;
    using System;
    using System.Xml;
    using System.IO;
    using System.Reflection;
    using System.ComponentModel;
    using System.CodeDom;
    using System.Text;
    using System.Web.Services.Configuration;
    using System.Diagnostics;

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription"]/*' />
    /// <devdoc>
    /// 
    /// </devdoc>
    [XmlRoot("definitions", Namespace=ServiceDescription.Namespace)]
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class ServiceDescription : DocumentableItem {
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public const string Namespace = "http://schemas.xmlsoap.org/wsdl/";
        Types types;
        ImportCollection imports;
        MessageCollection messages;
        PortTypeCollection portTypes;
        BindingCollection bindings;
        ServiceCollection services;
        string name;
        string targetNamespace;
        ServiceDescriptionFormatExtensionCollection extensions;
        ServiceDescriptionCollection parent;
        string appSettingUrlKey;
        string appSettingBaseUrl;
        string retrievalUrl;
        static XmlSerializer serializer;
        static XmlSerializerNamespaces namespaces;

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.RetrievalUrl"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public string RetrievalUrl {
            get { return retrievalUrl == null ? string.Empty : retrievalUrl; }
            set { retrievalUrl = value; }
        }

        internal void SetParent(ServiceDescriptionCollection parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.ServiceDescriptions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionCollection ServiceDescriptions {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Imports"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("import")]
        public ImportCollection Imports {
            get { if (imports == null) imports = new ImportCollection(this); return imports; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Types"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("types")]
        public Types Types {
            get { if (types == null) types = new Types(); return types; }
            set { types = value; }
        }

        private bool ShouldSerializeTypes() { return Types.HasItems(); }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Messages"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("message")]
        public MessageCollection Messages {
            get { if (messages == null) messages = new MessageCollection(this); return messages; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.PortTypes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("portType")]
        public PortTypeCollection PortTypes {
            get { if (portTypes == null) portTypes = new PortTypeCollection(this); return portTypes; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Bindings"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("binding")]
        public BindingCollection Bindings {
            get { if (bindings == null) bindings = new BindingCollection(this); return bindings; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Services"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("service")]
        public ServiceCollection Services {
            get { if (services == null) services = new ServiceCollection(this); return services; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions {
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.TargetNamespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("targetNamespace")]
        public string TargetNamespace {
            get { return targetNamespace; }
            set { targetNamespace = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NMTOKEN")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        // This is a special serializer that hardwires to the generated
        // ServiceDescriptionSerializer. To regenerate the serializer
        // Turn on KEEPTEMPFILES 
        // Restart server
        // Run wsdl as follows
        //   wsdl <URL_FOR_VALID_ASMX_FILE>?wsdl
        // Goto windows temp dir (usually \winnt\temp)
        // and get the latest generated .cs file
        // Change namespace to 'System.Web.Services.Description'
        // Change class names to ServiceDescriptionSerializationWriter
        // and ServiceDescriptionSerializationReader
        // Make the classes internal
        // Ensure the public Write method is Write108_definitions (If not
        // change Serialize to call the new one)
        // Ensure the public Read method is Read109_definitions (If not
        // change Deserialize to call the new one)
        internal class ServiceDescriptionSerializer : XmlSerializer {
            protected override XmlSerializationReader CreateReader() {
                return new ServiceDescriptionSerializationReader();
            }
            protected override XmlSerializationWriter CreateWriter() {
                return new ServiceDescriptionSerializationWriter();
            }
            public override bool CanDeserialize(System.Xml.XmlReader xmlReader){
                return xmlReader.IsStartElement("definitions", "http://schemas.xmlsoap.org/wsdl/");
            }
            protected override void Serialize(Object objectToSerialize, XmlSerializationWriter writer){
                ((ServiceDescriptionSerializationWriter)writer).Write108_definitions(objectToSerialize);
            }
            protected override object Deserialize(XmlSerializationReader reader){
                return ((ServiceDescriptionSerializationReader)reader).Read109_definitions();
            }
        }
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Serializer"]/*' />
        /// <devdoc>
        /// Returns the serializer for processing web service calls.  The serializer is customized according
        /// to settings in config.web.
        /// <internalonly/>
        /// <internalonly/>
        /// </devdoc>
        [XmlIgnore]
        public static XmlSerializer Serializer {
            get { 
                if (serializer == null) {
                    WebServicesConfiguration config = WebServicesConfiguration.Current;
                    XmlAttributeOverrides overrides = new XmlAttributeOverrides();
                    XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
                    ns.Add("s", XmlSchema.Namespace);
                    WebServicesConfiguration.LoadXmlFormatExtensions(config.ServiceDescriptionFormatExtensionTypes, overrides, ns);
                    namespaces = ns;
                    if (config.ServiceDescriptionExtended)
                        serializer = new XmlSerializer(typeof(ServiceDescription), overrides);
                    else
                        serializer = new ServiceDescriptionSerializer();
                }
                return serializer;
            }
        }

        internal string AppSettingBaseUrl {
            get { return appSettingBaseUrl; }
            set { appSettingBaseUrl = value; }
        }

        internal string AppSettingUrlKey {
            get { return appSettingUrlKey; }
            set { appSettingUrlKey = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Read"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static ServiceDescription Read(TextReader textReader) {
            XmlTextReader reader = new XmlTextReader(textReader);	
            reader.WhitespaceHandling = WhitespaceHandling.Significant;
            reader.XmlResolver = null;
            return Read(reader);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Read1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static ServiceDescription Read(Stream stream) {
            XmlTextReader reader = new XmlTextReader(stream);
            reader.WhitespaceHandling = WhitespaceHandling.Significant;
            reader.XmlResolver = null;
            return Read(reader);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Read2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static ServiceDescription Read(XmlReader reader) {
            return (ServiceDescription)Serializer.Deserialize(reader);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Read3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static ServiceDescription Read(string fileName) {
            StreamReader reader = new StreamReader(fileName, Encoding.Default, true);
            try {
                return Read(reader);
            }
            finally {
                reader.Close();
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.CanRead"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static bool CanRead(XmlReader reader) {
            return Serializer.CanDeserialize(reader);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Write"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Write(string fileName) {
            StreamWriter writer = new StreamWriter(fileName);
            try {
                Write(writer);
            }
            finally {
                writer.Close();
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Write1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Write(TextWriter writer) {
            XmlTextWriter xmlWriter = new XmlTextWriter(writer);
            xmlWriter.Formatting = Formatting.Indented;
            xmlWriter.Indentation = 2;
            Write(xmlWriter);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Write2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Write(Stream stream) {
            TextWriter writer = new StreamWriter(stream);
            Write(writer);
            writer.Flush();
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescription.Write3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Write(XmlWriter writer) {
            XmlSerializer serializer = Serializer;
            XmlSerializerNamespaces ns = new XmlSerializerNamespaces(namespaces);
            for (int i = 0; i < Types.Schemas.Count; i++) {
                string tns = Types.Schemas[i].TargetNamespace;
                if (tns != null && tns.Length > 0) {
                    ns.Add("s" + i.ToString(), tns);
                }
            }
            for (int i = 0; i < Imports.Count; i++) {
                Import import = Imports[i];
                if (import.Namespace.Length > 0) {
                    ns.Add("i" + i.ToString(), import.Namespace);
                }
            }
            if (this.TargetNamespace != null && this.TargetNamespace.Length != 0) {
                ns.Add("tns", this.TargetNamespace);
            }
            serializer.Serialize(writer, this, ns);
        }
    }


    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Import"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class Import : DocumentableItem {
        string ns;
        string location;
        ServiceDescription parent;

        internal void SetParent(ServiceDescription parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Import.ServiceDescription"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescription ServiceDescription {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Import.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("namespace")]
        public string Namespace {
            get { return ns == null ? string.Empty : ns; }
            set { ns = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Import.Location"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("location")]
        public string Location {
            get { return location == null ? string.Empty : location; }
            set { location = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="DocumentableItem"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public abstract class DocumentableItem {
        string documentation; 

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="DocumentableItem.Documentation"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("documentation"), DefaultValue("")]
        public string Documentation {
            get { return documentation == null ? string.Empty : documentation; }
            set { documentation = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Port"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class Port : DocumentableItem {
        ServiceDescriptionFormatExtensionCollection extensions;
        string name;
        XmlQualifiedName binding = XmlQualifiedName.Empty;
        Service parent;

        internal void SetParent(Service parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Port.Service"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Service Service {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Port.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions {
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Port.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Port.Binding"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("binding")]
        public XmlQualifiedName Binding {
            get { return binding; }
            set { binding = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Service"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class Service : DocumentableItem {
        ServiceDescriptionFormatExtensionCollection extensions;
        PortCollection ports;
        string name;
        ServiceDescription parent;

        internal void SetParent(ServiceDescription parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Service.ServiceDescription"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescription ServiceDescription {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Service.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions {
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Service.Ports"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("port")]
        public PortCollection Ports {
            get { if (ports == null) ports = new PortCollection(this); return ports; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Service.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBinding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class FaultBinding : MessageBinding {
        ServiceDescriptionFormatExtensionCollection extensions;

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBinding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public override ServiceDescriptionFormatExtensionCollection Extensions { 
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageBinding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public abstract class MessageBinding : DocumentableItem {
        OperationBinding parent;
        string name;

        internal void SetParent(OperationBinding parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageBinding.OperationBinding"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationBinding OperationBinding {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageBinding.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NMTOKEN")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageBinding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public abstract ServiceDescriptionFormatExtensionCollection Extensions { get; }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="InputBinding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class InputBinding : MessageBinding {
        ServiceDescriptionFormatExtensionCollection extensions; 

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="InputBinding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public override ServiceDescriptionFormatExtensionCollection Extensions { 
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OutputBinding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class OutputBinding : MessageBinding {
        ServiceDescriptionFormatExtensionCollection extensions; 

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OutputBinding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public override ServiceDescriptionFormatExtensionCollection Extensions { 
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class OperationBinding : DocumentableItem {
        ServiceDescriptionFormatExtensionCollection extensions; 
        string name;
        FaultBindingCollection faults;
        InputBinding input;
        OutputBinding output;
        Binding parent;

        internal void SetParent(Binding parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Binding"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Binding Binding {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions { 
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Input"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("input")]
        public InputBinding Input {
            get { return input; }
            set { input = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Output"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("output")]
        public OutputBinding Output {
            get { return output; }
            set { output = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBinding.Faults"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("fault")]
        public FaultBindingCollection Faults {
            get { if (faults == null) faults = new FaultBindingCollection(this); return faults; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class Binding : DocumentableItem {
        ServiceDescriptionFormatExtensionCollection extensions; 
        OperationBindingCollection operations; 
        string name;
        XmlQualifiedName type = XmlQualifiedName.Empty;
        ServiceDescription parent;

        internal void SetParent(ServiceDescription parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding.ServiceDescription"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescription ServiceDescription {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions { 
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding.Operations"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("operation")]
        public OperationBindingCollection Operations {
            get { if (operations == null) operations = new OperationBindingCollection(this); return operations; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Binding.Type"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("type")]
        public XmlQualifiedName Type {
            get { 
                if ((object)type == null) return XmlQualifiedName.Empty;
                return type; 
            }
            set { type = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessage"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public abstract class OperationMessage : DocumentableItem {
        string name;
        XmlQualifiedName message = XmlQualifiedName.Empty;
        Operation parent;

        internal void SetParent(Operation parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessage.Operation"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Operation Operation {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessage.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NMTOKEN")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessage.Message"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("message")]
        public XmlQualifiedName Message {
            get { return message; }
            set { message = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFault"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationFault : OperationMessage {
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationInput"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationInput : OperationMessage {
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationOutput"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationOutput : OperationMessage {
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class Operation : DocumentableItem {
        string name;
        string[] parameters;
        OperationMessageCollection messages;
        OperationFaultCollection faults;
        PortType parent;

        internal void SetParent(PortType parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.PortType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public PortType PortType {
            get { return parent; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.ParameterOrderString"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("parameterOrder"), DefaultValue("")]
        public string ParameterOrderString {
            get { 
                if (parameters == null) return string.Empty;
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < parameters.Length; i++) {
                    if (i > 0) builder.Append(' ');
                    builder.Append(parameters[i]);
                }
                return builder.ToString(); 
            }
            set {
                if (value == null)
                    parameters = null;
                else
                    parameters = value.Split(new char[] {' '});
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.ParameterOrder"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public string[] ParameterOrder {
            get { return parameters; }
            set { parameters = value; }
        }


        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.Messages"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("input", typeof(OperationInput)), 
        XmlElement("output", typeof(OperationOutput))]
        public OperationMessageCollection Messages {
            get { if (messages == null) messages = new OperationMessageCollection(this); return messages; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.Faults"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("fault")]
        public OperationFaultCollection Faults {
            get { if (faults == null) faults = new OperationFaultCollection(this); return faults; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Operation.IsBoundBy"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsBoundBy(OperationBinding operationBinding) {
            if (operationBinding.Name != Name) return false;
            OperationMessage input = Messages.Input;
            if (input != null) {
                if (operationBinding.Input == null) return false;

                string portTypeInputName = GetMessageName(Name, input.Name, true);
                string bindingInputName = GetMessageName(operationBinding.Name, operationBinding.Input.Name, true);
                if (bindingInputName != portTypeInputName) return false;
            }
            else if (operationBinding.Input != null)
                return false;
                
            OperationMessage output = Messages.Output;
            if (output != null) {
                if (operationBinding.Output == null) return false;

                string portTypeOutputName = GetMessageName(Name, output.Name, false);
                string bindingOutputName = GetMessageName(operationBinding.Name, operationBinding.Output.Name, false);
                if (bindingOutputName != portTypeOutputName) return false;
            }
            else if (operationBinding.Output != null)
                return false;
            return true;
        }

        private string GetMessageName(string operationName, string messageName, bool isInput){
            if (messageName != null && messageName.Length > 0)
                return messageName;
            
            switch (Messages.Flow) {
                case OperationFlow.RequestResponse:
                    if (isInput)
                        return operationName + "Request";
                    return operationName + "Response";
                case OperationFlow.OneWay:
                    if (isInput)
                        return operationName;
                    Debug.Assert(isInput == true, "Oneway flow cannot have an output message");
                    return null;
                /*Cases not supported
                case OperationFlow.SolicitResponse:
                    if (isInput)
                        return operationName + "Solicit";
                    return operationName + "Response";
                case OperationFlow.Notification:
                    if (!isInput)
                        return operationName;
                    Debug.Assert(isInput == false, "Notification flow cannot have an input message");
                    return null;
                 */
            }
            Debug.Assert(false, "Unknown message flow");
            return null;
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortType"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class PortType : DocumentableItem {
        string name;
        OperationCollection operations;
        ServiceDescription parent;

        internal void SetParent(ServiceDescription parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortType.ServiceDescription"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescription ServiceDescription {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortType.Operations"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("operation")]
        public OperationCollection Operations {
            get { if (operations == null) operations = new OperationCollection(this); return operations; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortType.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class Message : DocumentableItem {
        string name;
        MessagePartCollection parts;
        ServiceDescription parent;

        internal void SetParent(ServiceDescription parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message.ServiceDescription"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescription ServiceDescription {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message.Parts"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("part")]
        public MessagePartCollection Parts {
            get { if (parts == null) parts = new MessagePartCollection(this); return parts; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NCName")]
        public string Name {
            get { return name; }
            set { name = value; }
        }


        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message.FindPartsByName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public MessagePart[] FindPartsByName(string[] partNames) {
            MessagePart[] partArray = new MessagePart[partNames.Length];
            for (int i = 0; i < partNames.Length; i++) {
                partArray[i] = FindPartByName(partNames[i]);
            }
            return partArray;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Message.FindPartByName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public MessagePart FindPartByName(string partName) {
            for (int i = 0; i < parts.Count; i++) {
                MessagePart part = parts[i];
                if (part.Name == partName) return part;
            }
            throw new ArgumentException(Res.GetString(Res.MissingMessagePartForMessageFromNamespace3, partName, Name, ServiceDescription.TargetNamespace), "partName");
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePart"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class MessagePart : DocumentableItem {
        string name;
        XmlQualifiedName type = XmlQualifiedName.Empty;
        XmlQualifiedName element = XmlQualifiedName.Empty;
        Message parent;

        internal void SetParent(Message parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePart.Message"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Message Message {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePart.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name", DataType="NMTOKEN")]
        public string Name {
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePart.Element"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("element")]
        public XmlQualifiedName Element {
            get { return element; }
            set { element = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePart.Type"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("type")]
        public XmlQualifiedName Type {
            get { 
                if ((object)type == null) return XmlQualifiedName.Empty;
                return type; 
            }
            set { type = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Types"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [XmlFormatExtensionPoint("Extensions")]
    public sealed class Types : DocumentableItem {
        XmlSchemas schemas;
        ServiceDescriptionFormatExtensionCollection extensions;

        internal bool HasItems() { 
            return (schemas != null && schemas.Count > 0) ||
                (extensions != null && extensions.Count > 0);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Types.Schemas"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("schema", typeof(XmlSchema), Namespace=XmlSchema.Namespace)]
        public XmlSchemas Schemas {
            get { if (schemas == null) schemas = new XmlSchemas(); return schemas; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="Types.Extensions"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public ServiceDescriptionFormatExtensionCollection Extensions {
            get { if (extensions == null) extensions = new ServiceDescriptionFormatExtensionCollection(this); return extensions; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class ServiceDescriptionFormatExtensionCollection : ServiceDescriptionBaseCollection {
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.ServiceDescriptionFormatExtensionCollection"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ServiceDescriptionFormatExtensionCollection(object parent) : base(parent) { }

        ArrayList handledElements;
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public object this[int index] {
            get { return (object)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(object extension) {
            return List.Add(extension);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, object extension) {
            List.Insert(index, extension);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(object extension) {
            return List.IndexOf(extension);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(object extension) {
            return List.Contains(extension);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(object extension) {
            List.Remove(extension);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(object[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Find"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public object Find(Type type) {
            for (int i = 0; i < List.Count; i++) {
                object item = List[i];
                if (type.IsAssignableFrom(item.GetType())) {
                    ((ServiceDescriptionFormatExtension)item).Handled = true;
                    return item;
                }
            }
            return null;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.FindAll"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public object[] FindAll(Type type) {
            ArrayList list = new ArrayList();
            for (int i = 0; i < List.Count; i++) {
                object item = List[i];
                if (type.IsAssignableFrom(item.GetType())) {
                    ((ServiceDescriptionFormatExtension)item).Handled = true;
                    list.Add(item);
                }
            }
            return (object[])list.ToArray(type);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.Find1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlElement Find(string name, string ns) {
            for (int i = 0; i < List.Count; i++) {
                XmlElement element = List[i] as XmlElement;
                if (element != null && element.LocalName == name && element.NamespaceURI == ns) {
                    SetHandled(element);
                    return element;
                }
            }
            return null;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.FindAll1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlElement[] FindAll(string name, string ns) {
            ArrayList list = new ArrayList();
            for (int i = 0; i < List.Count; i++) {
                XmlElement element = List[i] as XmlElement;
                if (element != null && element.LocalName == name && element.NamespaceURI == ns) {
                    SetHandled(element);
                    list.Add(element);
                }
            }
            return (XmlElement[])list.ToArray(typeof(XmlElement));
        }

        void SetHandled(XmlElement element) {
            if (handledElements == null) 
                handledElements = new ArrayList();
            if (!handledElements.Contains(element))
                handledElements.Add(element);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.IsHandled"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsHandled(object item) {
            if (item is XmlElement)
                return IsHandled((XmlElement)item);
            else
                return ((ServiceDescriptionFormatExtension)item).Handled;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.IsRequired"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsRequired(object item) {
            if (item is XmlElement)
                return IsRequired((XmlElement)item);
            else
                return ((ServiceDescriptionFormatExtension)item).Required;
        }

        bool IsHandled(XmlElement element) {
            if (handledElements == null) return false;
            return handledElements.Contains(element);
        }

        bool IsRequired(XmlElement element) {
            XmlAttribute requiredAttr = element.Attributes["required", ServiceDescription.Namespace];
            if (requiredAttr == null || requiredAttr.Value == null) return false; // not required, by default
            return XmlConvert.ToBoolean(requiredAttr.Value);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            if (value is ServiceDescriptionFormatExtension) ((ServiceDescriptionFormatExtension)value).SetParent(parent);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtensionCollection.OnValidate"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnValidate(object value) {
            if (!(value is XmlElement || value is ServiceDescriptionFormatExtension)) 
                throw new ArgumentException(Res.GetString(Res.OnlyXmlElementsOrTypesDerivingFromServiceDescriptionFormatExtension0), "value");
            base.OnValidate(value);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtension"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public abstract class ServiceDescriptionFormatExtension {
        object parent; 
        bool required;
        bool handled;

        internal void SetParent(object parent) {
            this.parent = parent;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtension.Parent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public object Parent {
            get { return parent; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtension.Required"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("required", Namespace=ServiceDescription.Namespace), DefaultValue(false)]
        public bool Required {
            get { return required; }
            set { required = value; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionFormatExtension.Handled"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public bool Handled {
            get { return handled; }
            set { handled = value; }
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public enum OperationFlow {
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow.None"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        None,
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow.OneWay"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        OneWay,
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow.Notification"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Notification,
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow.RequestResponse"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        RequestResponse,
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFlow.SolicitResponse"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        SolicitResponse,
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationMessageCollection : ServiceDescriptionBaseCollection {
        internal OperationMessageCollection(Operation operation) : base(operation) { }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationMessage this[int index] {
            get { return (OperationMessage)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(OperationMessage operationMessage) {
            return List.Add(operationMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, OperationMessage operationMessage) {
            List.Insert(index, operationMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(OperationMessage operationMessage) {
            return List.IndexOf(operationMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(OperationMessage operationMessage) {
            return List.Contains(operationMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(OperationMessage operationMessage) {
            List.Remove(operationMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(OperationMessage[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Input"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationInput Input {
            get { 
                for (int i = 0; i < List.Count; i++) {
                    OperationInput input = List[i] as OperationInput;
                    if (input != null) {
                        return input;
                    }
                }
                return null;
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Output"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationOutput Output {
            get {
                for (int i = 0; i < List.Count; i++) {
                    OperationOutput output = List[i] as OperationOutput;
                    if (output != null) {
                        return output;
                    }
                }
                return null;
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.Flow"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationFlow Flow {
            get {
                if (List.Count == 0) {
                    return OperationFlow.None;
                }
                else if (List.Count == 1) {
                    if (List[0] is OperationInput) {
                        return OperationFlow.OneWay;
                    }
                    else {
                        return OperationFlow.Notification;
                    }
                }
                else {
                    if (List[0] is OperationInput) {
                        return OperationFlow.RequestResponse;
                    }
                    else {
                        return OperationFlow.SolicitResponse;
                    }
                }
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((OperationMessage)value).SetParent((Operation)parent);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.OnInsert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnInsert(int index, object value) {
            if (Count > 1 || (Count == 1 && value.GetType() == List[0].GetType()))
                throw new InvalidOperationException(Res.GetString(Res.WebDescriptionTooManyMessages));
            
            base.OnInsert(index, value);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.OnSet"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnSet(int index, object oldValue, object newValue) {
            if (oldValue.GetType() != newValue.GetType()) throw new InvalidOperationException(Res.WebDescriptionTooManyMessages);
            base.OnSet(index, oldValue, newValue);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationMessageCollection.OnValidate"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnValidate(object value) {
            if (!(value is OperationInput || value is OperationOutput))
                throw new ArgumentException(Res.GetString(Res.OnlyOperationInputOrOperationOutputTypes), "value");
            base.OnValidate(value);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class ImportCollection : ServiceDescriptionBaseCollection {
        internal ImportCollection(ServiceDescription serviceDescription) : base(serviceDescription) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Import this[int index] {
            get { return (Import)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Import import) {
            return List.Add(import);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Import import) {
            List.Insert(index, import);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Import import) {
            return List.IndexOf(import);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Import import) {
            return List.Contains(import);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Import import) {
            List.Remove(import);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Import[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ImportCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Import)value).SetParent((ServiceDescription)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class MessageCollection : ServiceDescriptionBaseCollection {
        internal MessageCollection(ServiceDescription serviceDescription) : base(serviceDescription) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Message this[int index] {
            get { return (Message)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Message message) {
            return List.Add(message);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Message message) {
            List.Insert(index, message);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Message message) {
            return List.IndexOf(message);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Message message) {
            return List.Contains(message);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Message message) {
            List.Remove(message);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Message[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Message this[string name] {
            get { return (Message)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((Message)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessageCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Message)value).SetParent((ServiceDescription)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class PortCollection : ServiceDescriptionBaseCollection {
        internal PortCollection(Service service) : base(service) { }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Port this[int index] {
            get { return (Port)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Port port) {
            return List.Add(port);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Port port) {
            List.Insert(index, port);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Port port) {
            return List.IndexOf(port);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Port port) {
            return List.Contains(port);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Port port) {
            List.Remove(port);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Port[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Port this[string name] {
            get { return (Port)Table[name]; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((Port)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Port)value).SetParent((Service)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class PortTypeCollection : ServiceDescriptionBaseCollection {
        internal PortTypeCollection(ServiceDescription serviceDescription) : base(serviceDescription) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public PortType this[int index] {
            get { return (PortType)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(PortType portType) {
            return List.Add(portType);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, PortType portType) {
            List.Insert(index, portType);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(PortType portType) {
            return List.IndexOf(portType);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(PortType portType) {
            return List.Contains(portType);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(PortType portType) {
            List.Remove(portType);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(PortType[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public PortType this[string name] {
            get { return (PortType)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((PortType)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="PortTypeCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((PortType)value).SetParent((ServiceDescription)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class BindingCollection : ServiceDescriptionBaseCollection {
        internal BindingCollection(ServiceDescription serviceDescription) : base(serviceDescription) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Binding this[int index] {
            get { return (Binding)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Binding binding) {
            return List.Add(binding);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Binding binding) {
            List.Insert(index, binding);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Binding binding) {
            return List.IndexOf(binding);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Binding binding) {
            return List.Contains(binding);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Binding binding) {
            List.Remove(binding);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Binding[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Binding this[string name] {
            get { return (Binding)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((Binding)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="BindingCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Binding)value).SetParent((ServiceDescription)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class ServiceCollection : ServiceDescriptionBaseCollection {
        internal ServiceCollection(ServiceDescription serviceDescription) : base(serviceDescription) { }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Service this[int index] {
            get { return (Service)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Service service) {
            return List.Add(service);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Service service) {
            List.Insert(index, service);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Service service) {
            return List.IndexOf(service);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Service service) {
            return List.Contains(service);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Service service) {
            List.Remove(service);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Service[] array, int index) {
            List.CopyTo(array, index);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Service this[string name] {
            get { return (Service)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((Service)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Service)value).SetParent((ServiceDescription)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class MessagePartCollection : ServiceDescriptionBaseCollection {
        internal MessagePartCollection(Message message) : base(message) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public MessagePart this[int index] {
            get { return (MessagePart)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(MessagePart messagePart) {
            return List.Add(messagePart);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, MessagePart messagePart) {
            List.Insert(index, messagePart);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(MessagePart messagePart) {
            return List.IndexOf(messagePart);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(MessagePart messagePart) {
            return List.Contains(messagePart);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(MessagePart messagePart) {
            List.Remove(messagePart);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(MessagePart[] array, int index) {
            List.CopyTo(array, index);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public MessagePart this[string name] {
            get { return (MessagePart)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((MessagePart)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="MessagePartCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((MessagePart)value).SetParent((Message)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationBindingCollection : ServiceDescriptionBaseCollection {
        internal OperationBindingCollection(Binding binding) : base(binding) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationBinding this[int index] {
            get { return (OperationBinding)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(OperationBinding bindingOperation) {
            return List.Add(bindingOperation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, OperationBinding bindingOperation) {
            List.Insert(index, bindingOperation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(OperationBinding bindingOperation) {
            return List.IndexOf(bindingOperation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(OperationBinding bindingOperation) {
            return List.Contains(bindingOperation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(OperationBinding bindingOperation) {
            List.Remove(bindingOperation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(OperationBinding[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationBindingCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((OperationBinding)value).SetParent((Binding)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class FaultBindingCollection : ServiceDescriptionBaseCollection {
        internal FaultBindingCollection(OperationBinding operationBinding) : base(operationBinding) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public FaultBinding this[int index] {
            get { return (FaultBinding)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(FaultBinding bindingOperationFault) {
            return List.Add(bindingOperationFault);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, FaultBinding bindingOperationFault) {
            List.Insert(index, bindingOperationFault);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(FaultBinding bindingOperationFault) {
            return List.IndexOf(bindingOperationFault);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(FaultBinding bindingOperationFault) {
            return List.Contains(bindingOperationFault);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(FaultBinding bindingOperationFault) {
            List.Remove(bindingOperationFault);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(FaultBinding[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public FaultBinding this[string name] {
            get { return (FaultBinding)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((FaultBinding)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="FaultBindingCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((FaultBinding)value).SetParent((OperationBinding)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationCollection : ServiceDescriptionBaseCollection {
        internal OperationCollection(PortType portType) : base(portType) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Operation this[int index] {
            get { return (Operation)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(Operation operation) {
            return List.Add(operation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, Operation operation) {
            List.Insert(index, operation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(Operation operation) {
            return List.IndexOf(operation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(Operation operation) {
            return List.Contains(operation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(Operation operation) {
            List.Remove(operation);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(Operation[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((Operation)value).SetParent((PortType)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class OperationFaultCollection : ServiceDescriptionBaseCollection {
        internal OperationFaultCollection(Operation operation) : base(operation) { }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationFault this[int index] {
            get { return (OperationFault)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(OperationFault operationFaultMessage) {
            return List.Add(operationFaultMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, OperationFault operationFaultMessage) {
            List.Insert(index, operationFaultMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(OperationFault operationFaultMessage) {
            return List.IndexOf(operationFaultMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(OperationFault operationFaultMessage) {
            return List.Contains(operationFaultMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(OperationFault operationFaultMessage) {
            List.Remove(operationFaultMessage);
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(OperationFault[] array, int index) {
            List.CopyTo(array, index);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public OperationFault this[string name] {
            get { return (OperationFault)Table[name]; }
        }
        
        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override string GetKey(object value) {
            return ((OperationFault)value).Name;
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="OperationFaultCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetParent(object value, object parent) {
            ((OperationFault)value).SetParent((Operation)parent);
        }
    }

    /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public abstract class ServiceDescriptionBaseCollection : CollectionBase {
        Hashtable table; // CONSIDER, better implementation
        object parent;

        internal ServiceDescriptionBaseCollection(object parent) {
            this.parent = parent;
        }

        void SetParents(object parent) {
            for (int i = 0; i < List.Count; i++) {
                SetParent(List[i], parent);
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.Table"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected virtual IDictionary Table { 
            get { if (table == null) table = new Hashtable(); return table; }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.GetKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected virtual string GetKey(object value) {
            return null; // returning null means there is no key
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.SetParent"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected virtual void SetParent(object value, object parent) {
            // default is that the item has no parent
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.OnInsertComplete"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnInsertComplete(int index, object value) {
            AddValue(value);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.OnRemove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnRemove(int index, object value) {
            RemoveValue(value);
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.OnClear"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnClear() {
            for (int i = 0; i < List.Count; i++) {
                RemoveValue(List[i]);
            }
        }

        /// <include file='doc\ServiceDescription.uex' path='docs/doc[@for="ServiceDescriptionBaseCollection.OnSet"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void OnSet(int index, object oldValue, object newValue) {
            RemoveValue(oldValue);
            AddValue(newValue);
        }
       
        void AddValue(object value) {
            string key = GetKey(value);
            if (key != null) {
                try {
                    Table.Add(key, value);
                }
                catch (Exception e) {
                    if (Table[key] != null) {
                        throw new ArgumentException(GetDuplicateMessage(value.GetType(), key.ToString()), e.InnerException);
                    }
                    else {
                        throw e;
                    }
                }
            }
            SetParent(value, parent);
        }

        void RemoveValue(object value) {
            string key = GetKey(value);
            if (key != null) Table.Remove(key);
            SetParent(value, null);
        }

        static string GetDuplicateMessage(Type type, string elemName) {
            string message = null;
            if (type == typeof(ServiceDescriptionFormatExtension)) 
                message = Res.GetString(Res.WebDuplicateFormatExtension, elemName);
            else if (type == typeof(OperationMessage)) 
                message = Res.GetString(Res.WebDuplicateOperationMessage, elemName);
            else if (type == typeof(Import)) 
                message = Res.GetString(Res.WebDuplicateImport, elemName);
            else if (type == typeof(Message)) 
                message = Res.GetString(Res.WebDuplicateMessage, elemName);
            else if (type == typeof(Port)) 
                message = Res.GetString(Res.WebDuplicatePort, elemName);
            else if (type == typeof(PortType)) 
                message = Res.GetString(Res.WebDuplicatePortType, elemName);
            else if (type == typeof(Binding)) 
                message = Res.GetString(Res.WebDuplicateBinding, elemName);
            else if (type == typeof(Service)) 
                message = Res.GetString(Res.WebDuplicateService, elemName);
            else if (type == typeof(MessagePart)) 
                message = Res.GetString(Res.WebDuplicateMessagePart, elemName);
            else if (type == typeof(OperationBinding)) 
                message = Res.GetString(Res.WebDuplicateOperationBinding, elemName);
            else if (type == typeof(FaultBinding)) 
                message = Res.GetString(Res.WebDuplicateFaultBinding, elemName);
            else if (type == typeof(Operation)) 
                message = Res.GetString(Res.WebDuplicateOperation, elemName);
            else if (type == typeof(OperationFault)) 
                message = Res.GetString(Res.WebDuplicateOperationFault, elemName);
            else
                message = Res.GetString(Res.WebDuplicateUnknownElement, type, elemName);

            return message;
        }
    }
}
