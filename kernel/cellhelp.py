#!/usr/bin/env python3

தஃ __future__ இறக்குமதி annotations
இறக்குமதி fileinput
இறக்குமதி json
தஃ pathlib இறக்குமதி Path

வகுப்பு SimHelper:
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
        க்கு field உள் printed_fields:
            field_val = getattr(self, field)
            எனில் isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        திரும்பு val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    க்கு வரி உள் cell.desc:
        எனில் வரி.startswith("Truth table:"):
            indent = "   "
            new_desc.எடு()
            new_desc.extend(["::", ""])
        new_desc.append(indent + வரி)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

க்கு வரி உள் fileinput.உள்ளீடு():
    வரி = வரி.rstrip()
    # special comments
    எனில் வரி.startswith("//-"):
        simHelper.desc.append(வரி[4:] எனில் len(வரி) > 4 இல்லையெனில் "")
    அல்லதெனில் வரி.startswith("//* "):
        _, சாவி, val = வரி.split(maxsplit=2)
        setattr(simHelper, சாவி, val)

    # code parsing
    எனில் வரி.startswith("module "):
        clean_வரி = வரி[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_வரி.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    அல்லதெனில் இல்லை வரி.startswith("endmodule"):
        வரி = "    " + வரி
    முயல்:
        simHelper.code.append(வரி.replace("\t", "    "))
    விதிவிலக்கு AttributeError:
        # no module definition, ignore line
        pass
    எனில் வரி.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        எனில் simHelper.ver == "1" மற்றும் short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        எனில் simHelper.desc மற்றும் simHelper.ver == "1" மற்றும் short_filename == "simlib.v" மற்றும் simHelper.desc[1].startswith('    '):
            simHelper.desc.எடு(1)

        # check group
        ப்ரதிஜ்ஞா simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
