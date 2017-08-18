from xml.dom import minidom


def decodeValue(param_type,param_value):
    if param_type == "int":
        return int(param_value.firstChild.data)
    if param_type == "float":
        return float(param_value.firstChild.data)
    if param_type == "string":
        return str(param_value.firstChild.data)
    if param_type == "vector_int":
        to_return = []
        items = param_value.getElementsByTagName("item")
        for item in items:
            to_return.append(int(item.firstChild.data))
        return to_return
    if param_type == "vector_float":
        to_return = []
        items = param_value.getElementsByTagName("item")
        for item in items:
            to_return.append(float(item.firstChild.data))
        return to_return
    if param_type == "vector_string":
        to_return = []
        items = param_value.getElementsByTagName("item")
        for item in items:
            to_return.append(str(item.firstChild.data))
        return to_return

def parse_hawcnest_xml(infile):
    hnconf = minidom.parse(infile)

    services = hnconf.getElementsByTagName("service")

    hnconfiguration = []

    for service in services:
        serviceType = str(service.getElementsByTagName("type")[0].firstChild.data)
        serviceName = str(service.getElementsByTagName("name")[0].firstChild.data)
        params = service.getElementsByTagName("param")
        serviceConfiguration = {}
        for param in params:
            param_type = str(param.getElementsByTagName("type")[0].firstChild.data)
            param_name = str(param.getElementsByTagName("name")[0].firstChild.data)
            param_value = param.getElementsByTagName("value")[0]
            serviceConfiguration[param_name] = decodeValue(param_type,param_value)
        hnconfiguration.append( (serviceType,serviceName,serviceConfiguration) )

    return hnconfiguration


