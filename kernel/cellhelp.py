#!/usr/bin/env python3

तः __future__ आनयन annotations
आनयन fileinput
आनयन json
तः pathlib आनयन Path

वग्ग SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    source: str = ""
    desc: list[str]
    code: list[str]
    group: str = ""
    ver: str = "1"
    tags: list[str]

    def __init__(self) -> None:
        self.desc = []
        self.tags = []

    def __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        अत्थाय field अन्तः printed_fields:
            field_val = getattr(self, field)
            सचे isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        पटिनिवत्त val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    अत्थाय पङ्क्तिः अन्तः cell.desc:
        सचे पङ्क्तिः.startswith("Truth table:"):
            indent = "   "
            new_desc.निष्कासय()
            new_desc.extend(["::", ""])
        new_desc.append(indent + पङ्क्तिः)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

अत्थाय पङ्क्तिः अन्तः fileinput.निवेशः():
    पङ्क्तिः = पङ्क्तिः.rstrip()
    # special comments
    सचे पङ्क्तिः.startswith("//-"):
        simHelper.desc.append(पङ्क्तिः[4:] सचे len(पङ्क्तिः) > 4 अञ्ञथा "")
    नोचेद्यदि पङ्क्तिः.startswith("//* "):
        _, कुञ्चिका, val = पङ्क्तिः.split(maxsplit=2)
        setattr(simHelper, कुञ्चिका, val)

    # code parsing
    सचे पङ्क्तिः.startswith("module "):
        clean_पङ्क्तिः = पङ्क्तिः[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_पङ्क्तिः.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    नोचेद्यदि न पङ्क्तिः.startswith("endmodule"):
        पङ्क्तिः = "    " + पङ्क्तिः
    यत्न:
        simHelper.code.append(पङ्क्तिः.replace("\t", "    "))
    अपवाद AttributeError:
        # no module definition, ignore line
        pass
    सचे पङ्क्तिः.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        सचे simHelper.ver == "1" च short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        सचे simHelper.desc च simHelper.ver == "1" च short_filename == "simlib.v" च simHelper.desc[1].startswith('    '):
            simHelper.desc.निष्कासय(1)

        # check group
        प्रतिज्ञा simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
