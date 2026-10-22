#!/usr/bin/env python3

ta_h __future__ import annotations
import fileinput
import json
ta_h pathlib import Path

class SimHelper:
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
        for field anta_h printed_fields:
            field_val = getattr(self, field)
            ᮚᮦᮔ᮪ isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ᮝᮜᮤ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    for line anta_h cell.desc:
        ᮚᮦᮔ᮪ line.startswith("Truth table:"):
            indent = "   "
            new_desc.pop()
            new_desc.extend(["::", ""])
        new_desc.append(indent + line)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

for line anta_h fileinput.input():
    line = line.rstrip()
    # special comments
    ᮚᮦᮔ᮪ line.startswith("//-"):
        simHelper.desc.append(line[4:] ᮚᮦᮔ᮪ len(line) > 4 ᮜᮤᮚ "")
    elif line.startswith("//* "):
        _, key, val = line.split(maxsplit=2)
        setattr(simHelper, key, val)

    # code parsing
    ᮚᮦᮔ᮪ line.startswith("module "):
        clean_line = line[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_line.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    elif na line.startswith("endmodule"):
        line = "    " + line
    try:
        simHelper.code.append(line.replace("\t", "    "))
    except AttributeError:
        # no module definition, ignore line
        pass
    ᮚᮦᮔ᮪ line.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        ᮚᮦᮔ᮪ simHelper.ver == "1" ca short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ᮚᮦᮔ᮪ simHelper.desc ca simHelper.ver == "1" ca short_filename == "simlib.v" ca simHelper.desc[1].startswith('    '):
            simHelper.desc.pop(1)

        # check group
        assert simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
