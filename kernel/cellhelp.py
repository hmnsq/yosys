#!/usr/bin/env python3

તઃ __future__ આયાત annotations
આયાત fileinput
આયાત json
તઃ pathlib આયાત Path

વર્ગ SimHelper:
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
        માટે field માં printed_fields:
            field_val = getattr(self, field)
            જો isinstance(field_val, list):
                field_val = "\n".join(field_val)
            field_val = field_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        પાછા val

def simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    માટે લીટી માં cell.desc:
        જો લીટી.startswith("Truth table:"):
            indent = "   "
            new_desc.કાઢો()
            new_desc.extend(["::", ""])
        new_desc.append(indent + લીટી)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

માટે લીટી માં fileinput.આગત():
    લીટી = લીટી.rstrip()
    # special comments
    જો લીટી.startswith("//-"):
        simHelper.desc.append(લીટી[4:] જો len(લીટી) > 4 નહીંતર "")
    અથવાજો લીટી.startswith("//* "):
        _, ચાવી, val = લીટી.split(maxsplit=2)
        setattr(simHelper, ચાવી, val)

    # code parsing
    જો લીટી.startswith("module "):
        clean_લીટી = લીટી[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_લીટી.split(maxsplit=1)
        simHelper.code = []
        short_filename = Path(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    અથવાજો નહીં લીટી.startswith("endmodule"):
        લીટી = "    " + લીટી
    પ્રયાસ:
        simHelper.code.append(લીટી.replace("\t", "    "))
    અપવાદ AttributeError:
        # no module definition, ignore line
        pass
    જો લીટી.startswith("endmodule"):
        short_filename = Path(fileinput.filename()).name
        જો simHelper.ver == "1" અને short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        જો simHelper.desc અને simHelper.ver == "1" અને short_filename == "simlib.v" અને simHelper.desc[1].startswith('    '):
            simHelper.desc.કાઢો(1)

        # check group
        પ્રતિજ્ઞા simHelper.group, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        print(simHelper)
        # new
        simHelper = SimHelper()
