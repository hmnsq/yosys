#!/usr/bin/env python3

ತಃ __future__ ಆನಯನ annotations
ಆನಯನ fileinput
ಆನಯನ json
ತಃ pathlib ಆನಯನ Path

ವರ್ಗ SimHelper:
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
        ಪ್ರತಿ field ಅನ್ತಃ printed_fields:
            field_val = getattr(self, field)
            ಯದಿ isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ಪ್ರತ್ಯಾಗಮ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ cell.desc:
        ಯದಿ ಸಾಲು.startswith("Truth table:"):
            indent = "   "
            new_desc.ತೆಗೆ()
            new_desc.extend(["::", ""])
        new_desc.append(indent + ಸಾಲು)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ fileinput.ಒಳಹರಿವು():
    ಸಾಲು = ಸಾಲು.rstrip()
    # special comments
    ಯದಿ ಸಾಲು.startswith("//-"):
        simHelper.desc.append(ಸಾಲು[4:] ಯದಿ len(ಸಾಲು) > 4 ಅನ್ಯಥಾ "")
    ನೋಚೇದ್ಯದಿ ಸಾಲು.startswith("//* "):
        _, ಕೀಲಿ, val = ಸಾಲು.split(maxsplit=2)
        setattr(simHelper, ಕೀಲಿ, val)

    # code parsing
    ಯದಿ ಸಾಲು.startswith("module "):
        clean_ಸಾಲು = ಸಾಲು[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_ಸಾಲು.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    ನೋಚೇದ್ಯದಿ ನ ಸಾಲು.startswith("endmodule"):
        ಸಾಲು = "    " + ಸಾಲು
    ಯತ್ನ:
        simHelper.code.append(ಸಾಲು.replace("\t", "    "))
    ಅಪವಾದ AttributeError:
        # no module definition, ignore line
        pass
    ಯದಿ ಸಾಲು.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        ಯದಿ simHelper.ver == "1" ಚ short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ಯದಿ simHelper.desc ಚ simHelper.ver == "1" ಚ short_filename == "simlib.v" ಚ simHelper.desc[1].startswith('    '):
            simHelper.desc.ತೆಗೆ(1)

        # check group
        ಪ್ರತಿಜ್ಞಾ simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
