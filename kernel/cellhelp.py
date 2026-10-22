#!/usr/bin/env python3

तः __future__ आनयन annotations
आनयन fileinput
आनयन json
तः pathlib आनयन Path

वर्ग SimHelper:
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
        प्रति field अन्तः printed_fields:
            field_val = getattr(self, field)
            यदि isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        प्रत्यागम val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    प्रति पंक्ति अन्तः cell.desc:
        यदि पंक्ति.startswith("Truth table:"):
            indent = "   "
            new_desc.निकालो()
            new_desc.extend(["::", ""])
        new_desc.append(indent + पंक्ति)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

प्रति पंक्ति अन्तः fileinput.निवेश():
    पंक्ति = पंक्ति.rstrip()
    # special comments
    यदि पंक्ति.startswith("//-"):
        simHelper.desc.append(पंक्ति[4:] यदि len(पंक्ति) > 4 अन्यथा "")
    नोचेद्यदि पंक्ति.startswith("//* "):
        _, कुंजी, val = पंक्ति.split(maxsplit=2)
        setattr(simHelper, कुंजी, val)

    # code parsing
    यदि पंक्ति.startswith("module "):
        clean_पंक्ति = पंक्ति[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_पंक्ति.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    नोचेद्यदि न पंक्ति.startswith("endmodule"):
        पंक्ति = "    " + पंक्ति
    यत्न:
        simHelper.code.append(पंक्ति.replace("\t", "    "))
    अपवाद AttributeError:
        # no module definition, ignore line
        pass
    यदि पंक्ति.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        यदि simHelper.ver == "1" च short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        यदि simHelper.desc च simHelper.ver == "1" च short_filename == "simlib.v" च simHelper.desc[1].startswith('    '):
            simHelper.desc.निकालो(1)

        # check group
        प्रतिज्ञा simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
