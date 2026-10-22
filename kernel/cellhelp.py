#!/usr/bin/env python3

من __future__ استورد annotations
استورد fileinput
استورد json
من pathlib استورد Path

صنف SimHelper:
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
        لكل field في printed_fields:
            field_val = getattr(self, field)
            𐡄𐡍 isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        أرجع val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل line في cell.desc:
        𐡄𐡍 line.startswith("Truth table:"):
            indent = "   "
            new_desc.pop()
            new_desc.extend(["::", ""])
        new_desc.append(indent + line)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

لكل line في fileinput.input():
    line = line.rstrip()
    # special comments
    𐡄𐡍 line.startswith("//-"):
        simHelper.desc.append(line[4:] 𐡄𐡍 len(line) > 4 وإلا "")
    وإلاإذا line.startswith("//* "):
        _, key, val = line.split(maxsplit=2)
        setattr(simHelper, key, val)

    # code parsing
    𐡄𐡍 line.startswith("module "):
        clean_line = line[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_line.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا 𐡋𐡀 line.startswith("endmodule"):
        line = "    " + line
    حاول:
        simHelper.code.append(line.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        pass
    𐡄𐡍 line.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        𐡄𐡍 simHelper.ver == "1" 𐡅 short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        𐡄𐡍 simHelper.desc 𐡅 simHelper.ver == "1" 𐡅 short_filename == "simlib.v" 𐡅 simHelper.desc[1].startswith('    '):
            simHelper.desc.pop(1)

        # check group
        أكد simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
