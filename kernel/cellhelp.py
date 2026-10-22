#!/usr/bin/env python3

তঃ __future__ আনযন annotations
আনযন fileinput
আনযন json
তঃ pathlib আনযন Path

বর্গ SimHelper:
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
        প্রতি field অন্তঃ printed_fields:
            field_val = getattr(self, field)
            যদি isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        প্রত্যাগম val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    প্রতি সারি অন্তঃ cell.desc:
        যদি সারি.startswith("Truth table:"):
            indent = "   "
            new_desc.বের_করো()
            new_desc.extend(["::", ""])
        new_desc.append(indent + সারি)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

প্রতি সারি অন্তঃ fileinput.প্রবেশ():
    সারি = সারি.rstrip()
    # special comments
    যদি সারি.startswith("//-"):
        simHelper.desc.append(সারি[4:] যদি len(সারি) > 4 অন্যথা "")
    নোচেদ্যদি সারি.startswith("//* "):
        _, চাবি, val = সারি.split(maxsplit=2)
        setattr(simHelper, চাবি, val)

    # code parsing
    যদি সারি.startswith("module "):
        clean_সারি = সারি[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_সারি.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    নোচেদ্যদি ন সারি.startswith("endmodule"):
        সারি = "    " + সারি
    যত্ন:
        simHelper.code.append(সারি.replace("\t", "    "))
    অপবাদ AttributeError:
        # no module definition, ignore line
        pass
    যদি সারি.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        যদি simHelper.ver == "1" চ short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        যদি simHelper.desc চ simHelper.ver == "1" চ short_filename == "simlib.v" চ simHelper.desc[1].startswith('    '):
            simHelper.desc.বের_করো(1)

        # check group
        প্রতিজ্ঞা simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
