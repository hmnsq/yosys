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
        लागि field अन्तः printed_fields:
            field_val = getattr(self, field)
            यदि isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        फर्काऊ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    लागि पङ्क्ति अन्तः cell.desc:
        यदि पङ्क्ति.startswith("Truth table:"):
            indent = "   "
            new_desc.झिक()
            new_desc.extend(["::", ""])
        new_desc.append(indent + पङ्क्ति)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

लागि पङ्क्ति अन्तः fileinput.आगत():
    पङ्क्ति = पङ्क्ति.rstrip()
    # special comments
    यदि पङ्क्ति.startswith("//-"):
        simHelper.desc.append(पङ्क्ति[4:] यदि len(पङ्क्ति) > 4 अन्यथा "")
    अथवायदि पङ्क्ति.startswith("//* "):
        _, साँचो, val = पङ्क्ति.split(maxsplit=2)
        setattr(simHelper, साँचो, val)

    # code parsing
    यदि पङ्क्ति.startswith("module "):
        clean_पङ्क्ति = पङ्क्ति[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_पङ्क्ति.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    अथवायदि होइन पङ्क्ति.startswith("endmodule"):
        पङ्क्ति = "    " + पङ्क्ति
    यत्न:
        simHelper.code.append(पङ्क्ति.replace("\t", "    "))
    अपवाद AttributeError:
        # no module definition, ignore line
        pass
    यदि पङ्क्ति.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        यदि simHelper.ver == "1" र short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        यदि simHelper.desc र simHelper.ver == "1" र short_filename == "simlib.v" र simHelper.desc[1].startswith('    '):
            simHelper.desc.झिक(1)

        # check group
        प्रतिज्ञा simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
