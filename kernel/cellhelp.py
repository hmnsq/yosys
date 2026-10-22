#!/usr/bin/env python3

തഃ __future__ ഇറക്കുമതി annotations
ഇറക്കുമതി fileinput
ഇറക്കുമതി json
തഃ pathlib ഇറക്കുമതി Path

വർഗ്ഗം SimHelper:
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
        വേണ്ടി field ഇൽ printed_fields:
            field_val = getattr(self, field)
            എങ്കിൽ isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        തിരികെ val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    വേണ്ടി വരി ഇൽ cell.desc:
        എങ്കിൽ വരി.startswith("Truth table:"):
            indent = "   "
            new_desc.എടുക്കുക()
            new_desc.extend(["::", ""])
        new_desc.append(indent + വരി)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

വേണ്ടി വരി ഇൽ fileinput.നിവേശം():
    വരി = വരി.rstrip()
    # special comments
    എങ്കിൽ വരി.startswith("//-"):
        simHelper.desc.append(വരി[4:] എങ്കിൽ len(വരി) > 4 അല്ലെങ്കിൽ "")
    അല്ലെങ്കിലെങ്കിൽ വരി.startswith("//* "):
        _, താക്കോൽ, val = വരി.split(maxsplit=2)
        setattr(simHelper, താക്കോൽ, val)

    # code parsing
    എങ്കിൽ വരി.startswith("module "):
        clean_വരി = വരി[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_വരി.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    അല്ലെങ്കിലെങ്കിൽ അല്ല വരി.startswith("endmodule"):
        വരി = "    " + വരി
    ശ്രമിക്കുക:
        simHelper.code.append(വരി.replace("\t", "    "))
    അപവാദം AttributeError:
        # no module definition, ignore line
        pass
    എങ്കിൽ വരി.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        എങ്കിൽ simHelper.ver == "1" ഒപ്പം short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        എങ്കിൽ simHelper.desc ഒപ്പം simHelper.ver == "1" ഒപ്പം short_filename == "simlib.v" ഒപ്പം simHelper.desc[1].startswith('    '):
            simHelper.desc.എടുക്കുക(1)

        # check group
        പ്രതിജ്ഞാ simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
