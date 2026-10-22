#!/usr/bin/env python3

ତଃ __future__ ଆମଦାନୀ annotations
ଆମଦାନୀ fileinput
ଆମଦାନୀ json
ତଃ pathlib ଆମଦାନୀ Path

ଶ୍ରେଣୀ SimHelper:
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
        ପାଇଁ field ଭିତରେ printed_fields:
            field_val = getattr(self, field)
            ଯଦି isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ଫେରନ୍ତୁ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    ପାଇଁ ଧାଡ଼ି ଭିତରେ cell.desc:
        ଯଦି ଧାଡ଼ି.startswith("Truth table:"):
            indent = "   "
            new_desc.କାଢ଼()
            new_desc.extend(["::", ""])
        new_desc.append(indent + ଧାଡ଼ି)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

ପାଇଁ ଧାଡ଼ି ଭିତରେ fileinput.ନିବେଶ():
    ଧାଡ଼ି = ଧାଡ଼ି.rstrip()
    # special comments
    ଯଦି ଧାଡ଼ି.startswith("//-"):
        simHelper.desc.append(ଧାଡ଼ି[4:] ଯଦି len(ଧାଡ଼ି) > 4 ଅନ୍ୟଥା "")
    ନଚେତଯଦି ଧାଡ଼ି.startswith("//* "):
        _, ଚାବି, val = ଧାଡ଼ି.split(maxsplit=2)
        setattr(simHelper, ଚାବି, val)

    # code parsing
    ଯଦି ଧାଡ଼ି.startswith("module "):
        clean_ଧାଡ଼ି = ଧାଡ଼ି[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_ଧାଡ଼ି.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    ନଚେତଯଦି ନୁହେଁ ଧାଡ଼ି.startswith("endmodule"):
        ଧାଡ଼ି = "    " + ଧାଡ଼ି
    ଚେଷ୍ଟା:
        simHelper.code.append(ଧାଡ଼ି.replace("\t", "    "))
    ବ୍ୟତିକ୍ରମ AttributeError:
        # no module definition, ignore line
        pass
    ଯଦି ଧାଡ଼ି.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        ଯଦି simHelper.ver == "1" ଏବଂ short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ଯଦି simHelper.desc ଏବଂ simHelper.ver == "1" ଏବଂ short_filename == "simlib.v" ଏବଂ simHelper.desc[1].startswith('    '):
            simHelper.desc.କାଢ଼(1)

        # check group
        ପ୍ରତିଜ୍ଞା simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
