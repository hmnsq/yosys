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
            ܐܢ isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ܦܢܐ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    لكل line في cell.desc:
        ܐܢ line.startswith("Truth table:"):
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
    ܐܢ line.startswith("//-"):
        simHelper.desc.append(line[4:] ܐܢ len(line) > 4 ܐܚܪܢܐ "")
    وإلاإذا line.startswith("//* "):
        _, key, val = line.split(maxsplit=2)
        setattr(simHelper, key, val)

    # code parsing
    ܐܢ line.startswith("module "):
        clean_line = line[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_line.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    وإلاإذا ܠܐ line.startswith("endmodule"):
        line = "    " + line
    حاول:
        simHelper.code.append(line.replace("\t", "    "))
    استثناء AttributeError:
        # no module definition, ignore line
        pass
    ܐܢ line.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        ܐܢ simHelper.ver == "1" ܘ short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ܐܢ simHelper.desc ܘ simHelper.ver == "1" ܘ short_filename == "simlib.v" ܘ simHelper.desc[1].startswith('    '):
            simHelper.desc.pop(1)

        # check group
        أكد simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
