#!/usr/bin/env python3

তঃ __future__ আনযন annotations
আনযন fileinput
আনযন json
তঃ pathlib আনযন Path

শ্ৰেণী SimHelper:
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
        বাবে field অন্তঃ printed_fields:
            field_val = getattr(self, field)
            যদি isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ঘূৰাই val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    বাবে সাৰি অন্তঃ cell.desc:
        যদি সাৰি.startswith("Truth table:"):
            indent = "   "
            new_desc.বেৰ_কৰো()
            new_desc.extend(["::", ""])
        new_desc.append(indent + সাৰি)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

বাবে সাৰি অন্তঃ fileinput.প্ৰবেশ():
    সাৰি = সাৰি.rstrip()
    # special comments
    যদি সাৰি.startswith("//-"):
        simHelper.desc.append(সাৰি[4:] যদি len(সাৰি) > 4 অন্যথা "")
    নোচেদ্যদি সাৰি.startswith("//* "):
        _, চাবি, val = সাৰি.split(maxsplit=2)
        setattr(simHelper, চাবি, val)

    # code parsing
    যদি সাৰি.startswith("module "):
        clean_সাৰি = সাৰি[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_সাৰি.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    নোচেদ্যদি নহয় সাৰি.startswith("endmodule"):
        সাৰি = "    " + সাৰি
    যত্ন:
        simHelper.code.append(সাৰি.replace("\t", "    "))
    অপৱাদ AttributeError:
        # no module definition, ignore line
        pass
    যদি সাৰি.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        যদি simHelper.ver == "1" আৰু short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        যদি simHelper.desc আৰু simHelper.ver == "1" আৰু short_filename == "simlib.v" আৰু simHelper.desc[1].startswith('    '):
            simHelper.desc.বেৰ_কৰো(1)

        # check group
        প্রতিজ্ঞা simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
